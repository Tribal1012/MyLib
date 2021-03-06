#include "logger.h"

namespace GlobalLogger {
	GlobalLogger::Logger* GlobalLogger::Logger::pInstance = nullptr;

	/* LogStream */
	// Original source code
	char *
	LogStream::longlong_to_string(
		__out char * buf,
		__in unsigned long long n,
		__in int len,
		__in DWORD flag)
	{
		int pos = len;
		int negative = 0;

		if ((flag & SIGNEDFLAG) && (long long)n < 0) {
			negative = 1;
			n = (~n + 1);
		}
		buf[--pos] = 0;

		/* only do the math if the number is >= 10 */
		while (n >= 10) {
			int digit = n % 10;
			n /= 10;
			buf[--pos] = digit + '0';
		}
		buf[--pos] = (char)(n + '0');

		if (negative)
			buf[--pos] = '-';
		else if ((flag & SHOWSIGNFLAG))
			buf[--pos] = '+';

		return &buf[pos];
	}

	// Original source code
	char *
	LogStream::longlong_to_hexstring(
		__out char * buf,
		__in unsigned long long u,
		__in int len,
		__in DWORD flag)
	{
		int pos = len;

		static const char hextable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		static const char hextable_caps[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		const char *table;

		if ((flag & CAPSFLAG))
			table = hextable_caps;
		else
			table = hextable;
		buf[--pos] = 0;
		do {
			unsigned int digit = u % 16;
			u /= 16;

			buf[--pos] = table[digit];
		} while (u != 0);

		return &buf[pos];
	}

	// Original vsnprintf source code
	LogStream &
	LogStream::format(
		__in const char* fmt,
		...)
	{
		va_list va;
		char c;
		unsigned char uc;
		const char *s;
		unsigned long long n;
		int flags;
		unsigned int format_num;
		size_t chars_written = 0;
		char num_buffer[32] = { 0, };
#define OUTPUT_CHAR(c) do { (m_stream += c); chars_written++; if (chars_written + 1 == INT_MAX) goto done; } while(0)
#define OUTPUT_CHAR_NOLENCHECK(c) do { (m_stream += c); chars_written++; } while(0)

		va_start(va, fmt);
		for (;;) {
			/* handle regular chars that aren't format related */
			while ((c = *fmt++) != 0) {
				if (c == '%')
					break; /* we saw a '%', break and start parsing format */
				OUTPUT_CHAR(c);
			}
			/* make sure we haven't just hit the end of the string */
			if (c == 0)
				break;
			/* reset the format state */
			flags = 0;
			format_num = 0;
		next_format:
			/* grab the next format character */
			c = *fmt++;
			if (c == 0)
				break;

			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (c == '0' && format_num == 0)
					flags |= LEADZEROFLAG;
				format_num *= 10;
				format_num += c - '0';
				goto next_format;
			case '.':
				/* XXX for now eat numeric formatting */
				goto next_format;
			case '%':
				OUTPUT_CHAR('%');
				break;
			case 'c':
				uc = va_arg(va, unsigned int);
				OUTPUT_CHAR(uc);
				break;
			case 's':
				if (flags & LONGLONGFLAG) s = (const char*)va_arg(va, const WCHAR *);
				else if (flags & LONGFLAG) s = (const char*)va_arg(va, const WCHAR *);
				else s = va_arg(va, const char *);
				if (s == 0)
					s = "<null>";
				goto _output_string;
			case '-':
				flags |= LEFTFORMATFLAG;
				goto next_format;
			case '+':
				flags |= SHOWSIGNFLAG;
				goto next_format;
			case '#':
				flags |= ALTFLAG;
				goto next_format;
			case 'l':
				if (flags & LONGFLAG)
					flags |= LONGLONGFLAG;
				flags |= LONGFLAG;
				goto next_format;
			case 'h':
				if (flags & HALFFLAG)
					flags |= HALFHALFFLAG;
				flags |= HALFFLAG;
				goto next_format;
			case 'D':
				flags |= LONGFLAG;
				/* fallthrough */
			case 'd':
				n = (flags & LONGLONGFLAG) ? va_arg(va, long long) :
					(flags & LONGFLAG) ? va_arg(va, long) :
					(flags & HALFHALFFLAG) ? (signed char)va_arg(va, int) :
					(flags & HALFFLAG) ? (short)va_arg(va, int) :
					(flags & SIZETFLAG) ? va_arg(va, SSIZE_T) :
					va_arg(va, int);
				flags |= SIGNEDFLAG;
				s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags);
				goto _output_string;
			case 'U':
				flags |= LONGFLAG;
				/* fallthrough */
			case 'u':
				n = (flags & LONGLONGFLAG) ? va_arg(va, unsigned long long) :
					(flags & LONGFLAG) ? va_arg(va, unsigned long) :
					(flags & HALFHALFFLAG) ? (unsigned char)va_arg(va, unsigned int) :
					(flags & HALFFLAG) ? (unsigned short)va_arg(va, unsigned int) :
					(flags & SIZETFLAG) ? va_arg(va, size_t) :
					va_arg(va, unsigned int);
				s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags);
				goto _output_string;
			case 'p':
				flags |= LONGFLAG | ALTFLAG;
				goto hex;
			case 'X':
				flags |= CAPSFLAG;
				/* fallthrough */
			hex:
			case 'x':
				n = (flags & LONGLONGFLAG) ? va_arg(va, unsigned long long) :
					(flags & LONGFLAG) ? va_arg(va, unsigned long) :
					(flags & HALFHALFFLAG) ? (unsigned char)va_arg(va, unsigned int) :
					(flags & HALFFLAG) ? (unsigned short)va_arg(va, unsigned int) :
					(flags & SIZETFLAG) ? va_arg(va, size_t) :
					va_arg(va, unsigned int);
				s = longlong_to_hexstring(num_buffer, n, sizeof(num_buffer), flags);
				if (flags & ALTFLAG) {
					OUTPUT_CHAR('0');
					OUTPUT_CHAR((flags & CAPSFLAG) ? 'X' : 'x');
				}
				goto _output_string;
			default:
				OUTPUT_CHAR('%');
				OUTPUT_CHAR(c);
				break;
			}
			/* move on to the next field */
			continue;
			/* shared output code */
		_output_string:
			if (flags & LEFTFORMATFLAG) {
				/* left justify the text */
				DWORD count = 0;
				while (*s != 0) {
					OUTPUT_CHAR(*s++);
					if (flags & LONGFLAG) s++;
					count++;
				}
				/* pad to the right (if necessary) */
				for (; format_num > count; format_num--)
					OUTPUT_CHAR(' ');
			}
			else {
				/* right justify the text (digits) */
				size_t string_len = strlen(s);
				char outchar = (flags & LEADZEROFLAG) ? '0' : ' ';
				for (; format_num > string_len; format_num--)
					OUTPUT_CHAR(outchar);
				/* output the string */
				while (*s != 0) {
					OUTPUT_CHAR(*s++);
					if (flags & LONGFLAG) s++;
				}
			}
			continue;
		}
	done:
		/* null terminate */
		// OUTPUT_CHAR_NOLENCHECK('\0');
		chars_written--; /* don't count the null */

		va_end(va);

#undef OUTPUT_CHAR
#undef OUTPUT_CHAR_NOLENCHECK

		return *this;
	}

	DWORD
	LogStream::SetDump(
		__in BYTE * pData,
		__in DWORD dwLen)
	{
		DWORD address = 0;
		DWORD row = 0;
		DWORD nread = 0;

		m_stream += "\r====================================================\n";
		while (1) {
			if (address >= dwLen) break;
			else format("%08x", address);
			nread = ((dwLen - address) > 16) ? 16 : (dwLen - address);

			// Show the hex codes
			for (DWORD i = 0; i < 16; i++)
			{
				if (i % 8 == 0) format(" ");
				if (i < nread) format(" %02x", (DWORD)pData[16 * row + i]);
				else format("   ");
			}

			// Show printable characters
			format("  ");
			for (DWORD i = 0; i < nread; i++)
			{
				if (pData[16 * row + i] < 32) format(".");
				else format("%c", pData[16 * row + i]);
			}

			format("\n");
			address += 16;
			row++;
		}

		m_stream += "\r====================================================\n";

		return 0;
	}

	VOID
	LogStream::Reset()
	{
		m_stream.clear();
	}

	BYTE *
	LogStream::GetStream()
	{
		return (BYTE*)m_stream.c_str();
	}

	DWORD
	LogStream::GetStreamLength()
	{
		return (DWORD)m_stream.size();
	}

	LogStream &
	LogStream::operator=(
		__in const char * data)
	{
		m_stream = data;

		return *this;
	}

	LogStream & 
	LogStream::operator=(
		__in const wchar_t * data)
	{
		std::wstring tmpWString = data;

		m_stream.assign(tmpWString.begin(), tmpWString.end());

		return *this;
	}

	LogStream &
	LogStream::operator=(
		__in LogStream & data)
	{
		m_stream = (char*)(data.GetStream());

		return *this;
	}

	LogStream &
	LogStream::operator+=(
		__in const char * data)
	{
		m_stream += data;

		return *this;
	}

	LogStream & 
	LogStream::operator+=(
		__in const wchar_t * data)
	{
		std::wstring tmpWString = data;
		std::string tmpString;

		tmpString.assign(tmpWString.begin(), tmpWString.end());

		m_stream += tmpString;

		return *this;
	}

	LogStream &
		LogStream::operator+=(
			__in LogStream & data)
	{
		m_stream += (char*)(data.GetStream());

		return *this;
	}

	LogStream
	LogStream::operator+(
		__in const char * data)
	{
		LogStream result = *this;
		result += data;
		return result;
	}

	LogStream 
	LogStream::operator+(
		__in const wchar_t * data)
	{
		LogStream result = *this;
		result += data;
		return result;
	}

	LogStream
	LogStream::operator+(
		__in LogStream & data)
	{
		LogStream result = *this;
		result += data;
		return result;
	}

	/* Logger */
	Logger *
	Logger::GetGlobalLogger(
		__in_opt BYTE* file)
	{
		if (!pInstance) {
			pInstance = new Logger(file);
			atexit(destroy);
		}

		return pInstance;
	}

	void
	Logger::destroy()
	{
		delete pInstance;
		pInstance = NULL;
	}

	Logger::Logger(
		__in BYTE* file)
	{
		std::string filename = (file) ? (char*)file : "";

		m_out = new std::ofstream();

		if (!file || Open(filename)) {
			delete m_out;
			m_out = NULL;
		}
	}

	Logger::~Logger()
	{
		Close();

		if (m_out) {
			delete m_out;
			m_out = NULL;
		}
	}

	DWORD
	Logger::Open(
		__in std::string& filename)
	{
		if (!m_out) m_out = new std::ofstream();
		if (m_out) {
			m_out->open(filename);
			if (!m_out->is_open())
				// Open file error
				return -1;

			// Success open
			return 0;
		}

		// Failed allocate ofstream object.
		return -2;
	}

	DWORD
	Logger::Write(
		__in LogStream * lStream)
	{
		if (m_out->is_open())
			*m_out << lStream->GetStream();
		else
			std::cout << "[-] Not opened..." << std::endl;

		return 0;
	}

	DWORD
	Logger::Write(
		__in LogStream & lStream)
	{
		if (m_out->is_open())
			*m_out << lStream.GetStream();
		else
			std::cout << "[-] Not opened..." << std::endl;

		return 0;
	}

	VOID
	Logger::Close()
	{
		if (m_out && m_out->is_open())
			m_out->close();
	}

	Logger &
	Logger::operator*(
		__in Logger & log)
	{
		return *this;
	}

	Logger &
	operator<<(
		__in Logger & log,
		__in LogStream & lstream)
	{
		BYTE* pStream = lstream.GetStream();
		DWORD dwStreamLen = lstream.GetStreamLength();

		log.Write(lstream);
		lstream.Reset();

		return log;
	}

	Logger &
	operator<<(
		__in Logger & log,
		__in LogStream * lstream)
	{
		BYTE* pStream = lstream->GetStream();
		DWORD dwStreamLen = lstream->GetStreamLength();

		log.Write(lstream);
		lstream->Reset();

		return log;
	}

	/* LogUtils(but not used...) */
	void
	LogUtils::hexdump(
		__in BYTE * p,
		__in DWORD len)
	{
		DWORD address = 0;
		DWORD row = 0;
		DWORD nread = 0;

		std::cout << std::hex << std::setfill('0');
		while (1) {
			if (address >= len) break;
			else std::cout << std::setw(8) << address;
			nread = ((len - address) > 16) ? 16 : (len - address);

			// Show the hex codes
			for (DWORD i = 0; i < 16; i++)
			{
				if (i % 8 == 0) std::cout << ' ';
				if (i < nread)
					std::cout << ' ' << std::setw(2) << (DWORD)p[16 * row + i];
				else
					std::cout << "   ";
			}

			// Show printable characters
			std::cout << "  ";
			for (DWORD i = 0; i < nread; i++)
			{
				if (p[16 * row + i] < 32) std::cout << '.';
				else std::cout << p[16 * row + i];
			}

			std::cout << "\n";
			address += 16;
			row++;
		}
		std::cout << std::dec;
	}

	void
	LogUtils::fhexdump(
		__in std::ofstream& out,
		__in BYTE * p,
		__in DWORD len)
	{
		std::streambuf *coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());

		hexdump(p, len);

		std::cout.rdbuf(coutbuf);
	}
}
