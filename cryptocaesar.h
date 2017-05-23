#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <utility>
#include <string>
#include <clocale>
#include <cctype>
#include <map>

class CryptoCaesar
{
    /* Разработано в среде: Microsoft Visual Studio 2010
     * Класс CryptoCaesar позвол¤ет осуществл¤ть шифрование/дешифрование текстовых данных, 
	 * которые содержат кириллические символы в кодировке windows-1251,
	 * используя алгоритм Цезаря с ключевым словом. */
public:

    CryptoCaesar(const std::string &keyWord, std::size_t offset):
      key(keyWord), offset(offset)
    {
        /* Устанавливаем локаль
         * Russian_Russia.1251 - идентификатор в формате POSIX*/
        setlocale(LC_ALL, "Russian_Russia.1251");

        /* Ключ формируем из уникальных символов*/
        std::sort(key.begin(), key.end());
        std::string::iterator last = std::unique(key.begin(), key.end());
        key.erase(last, key.end());

        /*Переводим ключ в верхний регистр*/
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);

        /* (N^2) Собираем последовательность кириллических символов
         * в верхнем регистре исключая символы ключа и
         * формируем исходный алфавит. */
        std::string charsWithoutKey; charsWithoutKey.reserve(sizeChars);
        for(std::size_t i = beginAsciiCode; i < endAsciiCode; ++i) {
            bool insert = true;
            for(std::size_t j = 0; j < key.length(); ++j)
                if((unsigned char)key[j] == (unsigned char)i){
                    insert = false; break;
                }
            if(insert) 
				charsWithoutKey.push_back((unsigned char)i); //последовательность без символов ключа
            srcChars.push_back((unsigned char)i);//исходный алфавит
        }

       /*Алфавит шифровани¤ с учетом смещени¤ и ключа*/
       modChars.resize(sizeChars);
       for(std::size_t i = 0, j = charsWithoutKey.length() - offset; i < sizeChars; ++i, ++j) {
           if(i == offset || i == key.length() + offset) j = 0;
           if(i < offset) modChars[i] = charsWithoutKey[j];
           if(i >= offset && i < key.length() + offset)
               modChars[i] = key[j];
           if(i >= key.length() + offset)
               modChars[i] = charsWithoutKey[j];
       }
    }
	
	/*Шифруем строку*/
    std::string encrypt(const std::string& input)
    {
        std::size_t length = input.length();
        std::string encryptStr; encryptStr.reserve(length);
        for(std::size_t i = 0; i < input.length(); ++i) {
           unsigned char ch = toupper(input[i]);
           size_t index = srcChars.find(ch);
           if(index == std::string::npos)
              encryptStr.push_back(input[i]);
           else {
              unsigned char mch = modChars[index];
              encryptStr.push_back(mch);
           }
        }
        return encryptStr;
    }

	/*Дешифруем строку*/
    std::string decrypt(const std::string& input)
    {
        std::size_t length = input.length();
        std::string decryptStr; decryptStr.reserve(length);
        for(std::size_t i = 0; i < input.length(); ++i) {
           unsigned char ch = input[i];
           size_t index = modChars.find(ch);
           if(index == std::string::npos)
              decryptStr.push_back(input[i]);
           else {
              unsigned char mch = srcChars[index];
              decryptStr.push_back(mch);
           }
        }
        return decryptStr;
    }

	/*Шифруем файл*/
    void encryptFile(const std::string& input, const std::string& output)
    {
        std::string encryptStr;
        std::ifstream file1(input.data(), std::ifstream::in);
        std::ofstream file2(output.data(), std::ofstream::out);
        for(std::string line; getline(file1, line);) {
            encryptStr = this->encrypt(line);
            file2<<encryptStr<<std::endl;
        }
        file1.close(); file2.close();
    }

	/*Дешифруем файл*/
    void decryptFile(const std::string& input, const std::string& output)
    {
        std::string decryptStr;
        std::ifstream file1(input.data(), std::ifstream::in);
        std::ofstream file2(output.data(), std::ofstream::out);
        for(std::string line; getline(file1, line);) {
            decryptStr = this->decrypt(line);
            file2<<decryptStr<<std::endl;
        }
        file1.close(); file2.close();
    }

	/*Частотный криптоанализ шифротекста*/
	void showFrequencyAnalys(const std::string& filename)
	{
		frequencyChars.clear();
		for(std::string::iterator it = modChars.begin(); it != modChars.end(); ++it)
			frequencyChars.insert(std::make_pair(*it, 0));

		std::ifstream file1(filename.data(), std::ifstream::in);
		for(std::string line; getline(file1, line);) {
			for(std::string::const_iterator its = line.cbegin(); its != line.cend(); ++its)
				if(frequencyChars.count(*its)) frequencyChars[*its]++;
        }

		file1.close();

		std::map<unsigned char, unsigned int>::iterator it;
		for(it = frequencyChars.begin(); it != frequencyChars.end(); ++it)
			std::cout << it->first << " -> " << it->second << std::endl;
	}

	virtual ~CryptoCaesar(){}

	void showSrcChars()
	{
		for(std::size_t i = 0; i < srcChars.length(); ++i)
			std::cout<<srcChars[i]<<' ';
		std::cout<<std::endl;
	}

	void showModChars()
	{
		for(std::size_t i = 0; i < srcChars.length(); ++i)
			std::cout<<modChars[i]<<' ';
		std::cout<<std::endl;
	}

private:

    /* Первый и слудующий за последним [192, 224)
     * Ascii коды кириллических букв в верхнем регистре
     * (кодировка 1251)*/
    static const int beginAsciiCode = 192;
    static const int endAsciiCode = 224;
    /* Количество символов в алфавите*/
    static const int sizeChars = 32;
    /* Отображение символ:количество*/
	std::map<unsigned char, std::size_t> frequencyChars;

    std::string key;
    std::size_t offset;
    std::string srcChars;
    std::string modChars;

};

/*
	CryptoCaesar obj1("ПАРОЛЬ", 3);
    obj1.encryptFile("data.txt", "encrypt.txt");

    std::cout<<"Частотный криптоанализ шифротекста: "<<std::endl;

	obj1.showSrcChars();
	obj1.showModChars();
	std::cout<<std::endl;
	obj1.showFrequencyAnalys("encrypt.txt");

    CryptoCaesar obj2("ПАРОЛЬ", 3);
    obj2.decryptFile("encrypt.txt", "decrypt.txt");
*/