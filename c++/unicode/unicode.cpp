/*
* Strings in C++
* 
* I.  Null-terminated byte strings
* --------------------------------
* A null-terminated byte string (NTBS) is a possibly empty sequence of nonzero bytes followed by a byte with value zero (the terminating null character).
* Each byte in a byte string encodes one character of some character set. For example, the character array {'\x63', '\x61', '\x74', '\0'} is an NTBS holding the string "cat" in ASCII encoding.
* Source: https://en.cppreference.com/w/cpp/string/byte
* 
* II. Null-terminated multibyte strings
* --------------------------------
* A null-terminated multibyte string (NTMBS), or "multibyte string", is a sequence of nonzero bytes followed by a byte with value zero (the terminating null character).
* Each character stored in the string may occupy more than one byte. The encoding used to represent characters in a multibyte character string is locale-specific: it may be UTF-8, GB18030, EUC-JP, Shift-JIS, etc.
* For example, the char array {'\xe4','\xbd','\xa0','\xe5','\xa5','\xbd','\0'} is an NTMBS holding the string "你好" in UTF-8 multibyte encoding: the first three bytes encode the character 你, the next three bytes encode the character 好. 
* The same string encoded in GB18030 is the char array {'\xc4', '\xe3', '\xba', '\xc3', '\0'}, where each of the two characters is encoded as a two-byte sequence.
* A multibyte character string is layout-compatible with null-terminated byte string (NTBS), that is, can be stored, copied, and examined using the same facilities, except for calculating the number of characters.
* If the correct locale is in effect, I/O functions also handle multibyte strings.
* Source: https://en.cppreference.com/w/cpp/string/multibyte
* 
* III. Null-terminated wide strings
* A null-terminated wide string is a sequence of valid wide characters, ending with a null character.
* Source: https://en.cppreference.com/w/cpp/string/wide
* 
*/


#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <cassert>

// Read the content of a UTF-8 encoded file
std::string read_utf8_file(const std::string& filename) {
	std::ifstream input_file(filename, std::ios::binary);
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(input_file)),
		std::istreambuf_iterator<char>()
	);
	return std::string(bytes.begin(), bytes.end());
}


// Process Unicode text (e.g., convert to uppercase)
std::u32string process_unicode_text(const std::u32string& input) {
	std::u32string result;
	for (char32_t ch : input) {
		//char32_t upper_case_ch = std::towupper(ch);
		char32_t upper_case_ch = ch;
		result.push_back(upper_case_ch);
	}
	return result;
}


std::size_t strlen_mb(const std::string_view s)
{
	std::mblen(nullptr, 0); // reset the conversion state
	std::size_t result = 0;
	const char* ptr = s.data();
	for (const char* const end = ptr + s.size(); ptr < end; ++result)
	{
		const int next = std::mblen(ptr, end - ptr);
		if (next == -1)
			throw std::runtime_error("strlen_mb(): conversion error");
		ptr += next;
	}
	return result;
}

void dump_bytes(const std::string_view str)
{
	std::cout << std::hex << std::uppercase << std::setfill('0');
	for (unsigned char c : str)
		std::cout << std::setw(2) << static_cast<int>(c) << ' ';
	std::cout << std::dec << '\n';
}

int main()
{
	char* locale;

	locale = setlocale(LC_ALL, "");

	// MILTIBYTE CHARS
	std::cout << "\nMILTIBYTE CHARS DEMO\n";
	const char *kArabicSampleText = u8"العربية";
	const char *kChineseSampleText = u8"中文";
	const char *kEnglishSampleText = u8"English";
	const char *kRussianSampleText = u8"Русский";

	const char* prog_r = kRussianSampleText;
	const char* prog_e = kEnglishSampleText;
	const char* prog_c = kChineseSampleText;
	// Set breakpoint here and add Watch for `prog_*` vars to inspect content.
	// Example: `prog_r,20` or `prog_r,s8`. s8 instruct to represent as unicode

	std::cout << "cout << kEnglishSampleText : " << kEnglishSampleText << std::endl;
	std::cout << "cout << kChineseSampleText : " << kChineseSampleText << std::endl;
	std::cout << "cout << kRussianSampleText : " << kRussianSampleText << std::endl;
	std::cout << std::endl;

	std::cout << "printf(\"%s\", kEnglishSampleText) : ";
	printf("%s\n", kEnglishSampleText);
	std::cout << "printf(\"%s\", kChineseSampleText) : ";
	printf("%s\n", kChineseSampleText);
	std::cout << "printf(\"%s\", kRussianSampleText) : ";
	printf("%s\n", kRussianSampleText);
	std::cout << std::endl;

	std::string str_repr = std::string(kRussianSampleText);
	std::cout << "Inspection of " << std::quoted(str_repr) << ":\n";
	std::cout << "\tCharacters: " << strlen_mb(str_repr) << "\n";
	std::cout << "\tBytes: " << str_repr.size() << "\n";
	std::cout << "\tInternal content: ";
	dump_bytes(kRussianSampleText);
	std::cout << std::endl;


	// WIDE CHARS
	std::cout << "\nWIDE CHARS DEMO\n";
	const wchar_t* kwEnglishSampleText = L"English";
	const wchar_t* kwChineseSampleText = L"中文";
	const wchar_t* kwRussianSampleText = L"Русский";

	std::wcout << "wcout << kwEnglishSampleText : " << kwEnglishSampleText << std::endl;
	std::wcout << "wcout << kwChineseSampleText : " << kwChineseSampleText << std::endl;
	std::wcout << "wcout << kwRussianSampleText : " << kwRussianSampleText << std::endl;
	std::cout << std::endl;

	std::cout << "printf(\"%ls\", kwEnglishSampleText) : ";
	printf("%ls\n", kwEnglishSampleText);
	std::cout << "printf(\"%ls\", kwChineseSampleText) : ";
	printf("%ls\n", kwChineseSampleText);
	std::cout << "printf(\"%ls\", kwRussianSampleText) : ";
	printf("%ls\n", kwRussianSampleText);
	std::cout << std::endl;

	std::wcout << "The length of \"" << kwRussianSampleText << "\" is " << std::wcslen(kwRussianSampleText) << '\n';
	std::cout << std::endl;


	// WIDESTRINGS
	std::cout << "\nWORKING WITH std::wstring\n";
	std::wstring wsEnglishSampleText = L"English";
	std::wstring wsChineseSampleText = L"中文";
	std::wstring wsRussianSampleText = L"Русский";

	std::wcout << L"std::wcout << wsEnglishSampleText : " << wsEnglishSampleText << std::endl;
	std::wcout << L"std::wcout << wsChineseSampleText : " << wsChineseSampleText << std::endl;
	std::wcout << L"std::wcout << wsRussianSampleText : " << wsRussianSampleText << std::endl;
	std::cout << std::endl;

	std::cout << "printf(\"%ls\", wsEnglishSampleText.c_str()) : ";
	printf("%ls\n", wsEnglishSampleText.c_str());
	std::cout << "printf(\"%ls\", wsChineseSampleText.c_str()) : ";
	printf("%ls\n", wsChineseSampleText.c_str());
	std::cout << "printf(\"%ls\", wsRussianSampleText.c_str()) : ";
	printf("%ls\n", wsRussianSampleText.c_str());
	std::cout << std::endl;


	std::cout << "Iterating over wide string\n";
	for (auto ch : wsRussianSampleText) {
		std::wcout << L"[" << ch << L"]" << " ";
	}
	std::cout << std::endl;

	std::wstring ws1 = L"Пример1";
	std::wstring ws2 = L"Пример2";
	std::wstring ws_token = L"имер";
	assert(ws1 == ws1);
	assert(ws1 < ws2);
	assert(ws1.find(ws_token) != ws1.npos);


	// CONVERT MULTIBYTE TO WIDESTRING
	std::cout << "\nCONVERT MULTIBYTE TO WIDESTRING\n";
	std::mbstate_t state = std::mbstate_t();

	const char* mbstr = kRussianSampleText;		// Source multibyte string
	std::cout << "\tMultibyte string: " << mbstr << "\n";

	// Firstly, obtain length of resulting multibyte string
	std::size_t len = std::mbsrtowcs(nullptr, &mbstr, 0, &state);

	// Now define wide string of the required size and convert mbstr to wide str
	std::wstring wstr;
	wstr.resize(len);
	std::mbsrtowcs(&wstr[0], &mbstr, wstr.size(), &state);

	// Alternatively, wchar vestor can be used instead of wstring
	//std::vector<wchar_t> wstr(len);
	//std::mbsrtowcs(&wstr[0], &mbstr, wstr.size(), &state);

	std::wcout << L"\tWide string: " << wstr << "\n";


	// UNICODE FILE READ EXAMPLE
	std::cout << "\nUNICODE FILE READING\n";

	// Read a UTF-8 encoded file
	std::string utf8_content = read_utf8_file("unicode.txt");


	// Convert UTF-8 content to UTF-32
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
	std::u32string utf32_content = converter.from_bytes(utf8_content);


	// Process Unicode text
	std::u32string processed_content = process_unicode_text(utf32_content);


	// Convert the result back to UTF-8
	std::string output_utf8_content = converter.to_bytes(processed_content);


	// Write the output to stdout
	std::cout << output_utf8_content << std::endl;
}
