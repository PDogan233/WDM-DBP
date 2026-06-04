#pragma once

#include"tool.h"
#include"vec.h"
#include<fstream>
#include<stdexcept>
#include<string>
#include<cstring>
#include<type_traits>
#include<cstdint>
#include<charconv>

namespace ZK
{
	enum class ValueType :std::uint8_t
	{
		Binary = 1,
		Int = 2,
		Double = 3,
		ComplexDouble = 4,
		String = 5
	};

	namespace mode
	{
		enum WriteMode
		{
			OverWrite = std::ios::trunc,
			Append = std::ios::app
		};
		enum FileType
		{
			Mat,
			Csv,
			Dat,
			Txt,
			Zat
		};
	}

	enum class MatDataType : uint32_t
	{
		miINT8 = 1,
		miUINT8 = 2,
		miINT16 = 3,
		miUINT16 = 4,
		miINT32 = 5,
		miUINT32 = 6,
		miSINGLE = 7,
		miDOUBLE = 9,
		miINT64 = 12,
		miUINT64 = 13,
		miMATRIX = 14,
		miCOMPRESSED = 15,
		miUTF8 = 16,
		miUTF16 = 17,
		miUTF32 = 18
	};

	enum class MatArrayType : uint8_t {
		mxDOUBLE_CLASS = 6,
		mxSINGLE_CLASS = 7,
		mxINT8_CLASS = 8,
		mxUINT8_CLASS = 9,
		mxINT16_CLASS = 10,
		mxUINT16_CLASS = 11,
		mxINT32_CLASS = 12,
		mxUINT32_CLASS = 13,
		mxINT64_CLASS = 14,
		mxUINT64_CLASS = 15
	};

	template<typename>
	struct ValueTypeTag;

	template<> struct ValueTypeTag<bin>
	{
		static constexpr ValueType value = ValueType::Binary;
	};
	template<> struct ValueTypeTag<int>
	{
		static constexpr ValueType value = ValueType::Int;
	};
	template<> struct ValueTypeTag <double>
	{
		static constexpr ValueType value = ValueType::Double;
	};
	template<> struct ValueTypeTag<std::complex <double>>
	{
		static constexpr ValueType value = ValueType::ComplexDouble;
	};
	template<> struct ValueTypeTag<std::string>
	{
		static constexpr ValueType value = ValueType::String;
	};


	/*!
	* value type of data
	* Function name: valueTypeOf
	* @param void
	* @return ValueType : The ValueType corresponding to type T
	* Function: Get the ValueType corresponding to type T
	*/
	template<typename T>
	constexpr ValueType valueTypeOf()
	{
		return ValueTypeTag<T>::value;
	}

	inline constexpr std::uint32_t getKMagic() { return 0x5A4B494F; }//"ZKIO"
	inline constexpr std::uint8_t getKVersion() { return 1; }
	inline constexpr std::size_t getKHeaderSize() { return sizeof(std::uint32_t) + sizeof(std::uint8_t) + sizeof(ValueType) + sizeof(std::uint64_t); }


	inline size_t padding8(size_t size) {
		return (8 - (size % 8)) % 8;
	}

	inline std::string getMatDataTypeStr(ValueType dataType)
	{
		switch (dataType)
		{
		case ValueType::Binary: { return "binary"; }
		case ValueType::Int: { return "int"; }
		case ValueType::Double: { return "double"; }
		case ValueType::ComplexDouble: { return "complex"; }
		case ValueType::String: { return "string"; }
		default: { return "unknow"; }
		}
	}

	/*!
	* Mat data type and array type function
	* Function name: getMatTypes
	* @param dataType : The MatDataType corresponding to type T
	* @param arrayType : The MatArrayType corresponding to type T
	* @return void
	* Function: Get the MatDataType and MatArrayType corresponding to type T
	*/
	template<typename T>
	inline void getMatTypes(MatDataType& dataType, MatArrayType& arrayType) {
		if (std::is_same<T, double>::value) {
			dataType = MatDataType::miDOUBLE;
			arrayType = MatArrayType::mxDOUBLE_CLASS;
		}
		else if (std::is_same<T, int8_t>::value || std::is_same<T, signed char>::value) {
			dataType = MatDataType::miINT8;
			arrayType = MatArrayType::mxINT8_CLASS;
		}

		else if (std::is_same<T, int32_t>::value || std::is_same<T, int>::value) {
			dataType = MatDataType::miINT32;
			arrayType = MatArrayType::mxINT32_CLASS;
		}
		else {
			dataType = MatDataType::miDOUBLE;
			arrayType = MatArrayType::mxDOUBLE_CLASS;
		}
	}

	/*!
	* Ensure data extension function
	* Function name: ensureZatExtension
	* @param fileName : The path and file name need to be ensured
	* @return std::string : The file name ensured to have .zat extension
	* Function: If the fileName already has a .zat extension, return it directly; otherwise, add a .zat extension and return it.
	*/
	inline std::string ensureZatExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".zat")
		{
			return fileName;
		}
		return fileName + ".zat";
	}

	/*!
	* Ensure data extension function
	* Function name: ensureDatExtension
	* @param fileName : The path and file name need to be ensured
	* @return std::string : The file name ensured to have .dat extension
	* Function: If the fileName already has a .dat extension, return it directly; otherwise, add a .dat extension and return it.
	*/
	inline std::string ensureDatExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".dat")
		{
			return fileName;
		}
		return fileName + ".dat";
	}

	/*!
	* Ensure data extension function
	* Function name: ensureTxtExtension
	* @param fileName : The path and file name need to be ensured
	* @return std::string : The file name ensured to have .txt extension
	* Function: If the fileName already has a .txt extension, return it directly; otherwise, add a .txt extension and return it.
	*/
	inline std::string ensureTxtExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".txt")
		{
			return fileName;
		}
		return fileName + ".txt";
	}

	/*!
	* Ensure data extension function
	* Function name: ensureCsvExtension
	* @param fileName : The path and file name need to be ensured
	* @return std::string : The file name ensured to have .csv extension
	* Function: If the fileName already has a .csv extension, return it directly; otherwise, add a .csv extension and return it.
	*/
	inline std::string ensureCsvExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".csv")
		{
			return fileName;
		}
		return fileName + ".csv";
	}


	/*!
	* Ensure data extension function
	* Function name: ensureTxtExtension
	* @param fileName : The path and file name need to be ensured
	* @return std::string : The file name ensured to have .mat extension
	* Function: If the fileName already has a .mat extension, return it directly; otherwise, add a .mat extension and return it.
	*/
	inline std::string ensureMatExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".mat")
		{
			return fileName;
		}
		return fileName + ".mat";
	}

	//-------------------------------------Vector saving function-------------------------------------------

	/*!
	* Memory data saving function
	* Function name: saveData
	* @param data : The starting memory address where the required data needs to be stored
	* @param size : The number of data items that need to be saved
	* @param fileName : The path and file name saved to the hard drive
	* @param fileType : The data format mode, "Bin", "Txt", etc.
	* @param writeMode : The file writing mode, "OverWrite" or "Append"
	* @prarm isNeedHeader : Whether to save the header information
	* @return void
	* Function: Save the size number of T-type data at the data address in the memory to the .dat file named fileName.
	*/
	template<typename T>
	void saveData(const T* data, std::size_t size, const std::string& fileName, int fileType = mode::Mat, int writeMode = mode::OverWrite, bool isNeedHeader = false);

	/*!
	* Vec<T> data saving function
	* Function name: saveData
	* @param data : Vec where the required data needs to be stored
	* @param fileName : The path and file name saved to the hard drive
	* @param fileType : The data format to be saved,"Bin", "Txt", etc.
	* @param writeMode : The file writing mode, "OverWrite" or "Append"
	* @return void
	* Function: Save the size number of T-type data at the data address in the memory to the .dat file named fileName.
	*/
	template<typename T>
	void saveData(const Vec<T>& data, const std::string& fileName, int fileType = mode::Mat, int writeMode = mode::OverWrite, bool isNeedHeader = false);

	/*!
	* Mat class data from memory saving function
	* Function name: saveData
	* @param data : The starting memory address where the required data needs to be stored
	* @param rows : The number of rows of mat data to be saved
	* @param cols : The number of columns of mat data to be saved
	* @param fileName : The path and file name saved to the hard drive
	* @param fileType : The data format to be saved,"Bin", "Txt", etc.
	* @param writeMode : The file writing mode, "OverWrite" or "Append"
	* @prarm isNeedHeader : Whether to save the header information
	* @return void
	* Function: Save the rows*cols number of T-type data at the data address in the memory to the .dat file named fileName.
	*/
	template<typename T>
	void saveData(const T* data, size_t rows, size_t cols, const std::string& fileName, int fileType = mode::Mat, int writeMode = mode::OverWrite, bool isNeedHeader = false);

	/*!
	* Mat<T> data saving function
	* Function name: saveData
	* @param data : Mat where the required data needs to be stored
	* @param fileName : The path and file name saved to the hard drive
	* @param fileType : The data format mode, "Bin", "Txt", etc.
	* @param writeMode : The file writing mode, "OverWrite" or "Append"
	* @prarm isNeedHeader : Whether to save the header information
	* @return void
	* Function: Save the Mat<T> type data to the .dat file named fileName.
	*/
	template<typename T>
	void saveData(const Mat<T>& data, const std::string& fileName, int fileType = mode::Mat, int writeMode = mode::OverWrite, bool isNeedHeader = false);

	//-----------------------------------------Vector loading function-------------------------------------------

	/*!
	* Get file type from extension
	* Function name: getFileTypeFromExtension
	* @param fileName : The file name to check
	* @return mode::FileType : The file type based on extension
	* Function: Determine the file type based on the file extension.
	*/
	inline mode::FileType getFileTypeFromExtension(const std::string& fileName)
	{
		if (fileName.size() >= 4)
		{
			std::string ext = fileName.substr(fileName.size() - 4);
			if (ext == ".mat") return mode::Mat;
			if (ext == ".csv") return mode::Csv;
			if (ext == ".dat") return mode::Dat;
			if (ext == ".txt") return mode::Txt;
			if (ext == ".zat") return mode::Zat;
		}
		return mode::Dat;  // default
	}

	/*!
	* Skip MAT file header and get data info
	* Function name: skipMatHeader
	* @param file : The input file stream
	* @param dataCount : Output parameter for the number of data elements
	* @param isComplex : Output parameter indicating if data is complex
	* @return bool : true if successful, false otherwise
	* Function: Skip the MAT file header (128 bytes) and parse the matrix structure to get data info.
	*/
	bool skipMatHeader(std::ifstream& file, size_t& dataCount, bool& isComplex);

	/*!
	* Skip text file header (lines starting with #)
	* Function name: skipTextHeader
	* @param file : The input file stream
	* @param dataCount : Output parameter for the number of data elements (if found in header)
	* @return bool : true if count was found in header, false otherwise
	* Function: Skip comment lines starting with # and try to extract data count from header.
	*/
	bool skipTextHeader(std::ifstream& file, size_t& dataCount);

	/*!
	* Memory data loading function
	* Function name: loadData
	* @param fileName : The path and file name saved to the hard drive
	* @param count : The number of data items that need to be loaded
	* @return Vec<T> : The Vec<T> object containing the loaded data
	* Function: Load count number of T-type data from the file named fileName into a Vec<T> object.
	*           Automatically detects file type and skips appropriate headers.
	*/
	template<typename T>
	Vec<T> loadData(const std::string& fileName, size_t count);

	/*!
	* Vec<T> data loading function with auto header detection
	* Function name: loadData
	* @param fileName : The path and file name saved to the hard drive
	* @return Vec<T> : The Vec<T> object containing the loaded data
	* Function: Load T-type data from the file named fileName into a Vec<T> object.
	*           Automatically detects file type and reads data count from header.
	*/
	template<typename T>
	Vec<T> loadData(const std::string& fileName);

	/*
* Load a DAT file into a Mat<T> object.
* Function name: loadDatMat
* @param fileName : The path and file name of the .dat file to be loaded
* @return Mat<T> : The Mat<T> object containing the loaded matrix data
* Function: Load a .dat file containing matrix data into a Mat<T> object.
*           The .dat file should have columns of data, with optional header lines starting with # that can specify rows and cols.
*			If no header is present, the function will attempt to infer the matrix dimensions from the data format.
*/
	template<typename T>
	Mat<T> loadDatMat(const std::string& fileName);

	/*!
	* create folder if not exist
	* Function name: createFolder
	* @param folderPath : The folder path to be created
	* @return bool : true if the folder is created or already exists, false otherwise
	* Function: Create the folder at the specified path if it does not already exist.
	*/
	ZKIC_DLL_EXPORT bool createFolder(const std::string& folderPath);


	/*!
	* write .mat file header (128 bytes)
	* Function name: writeMatHeader
	* @param file : The file to be written to
	* @return void
	* Function: Write the 128-byte MAT file header.
	*/
	void writeMatHeader(std::ofstream& file);

	/*!
	* write .mat file tag
	* Function name: writeTag
	* @param file : The file to be written to
	* @param type : The data type of the element
	* @param numBytes : The number of bytes of the data to be written
	* @return void
	* Function: Standard data element tag (8 bytes) used for writing MAT files.
	*/
	void writeTag(std::ofstream& file, MatDataType type, uint32_t numBytes);


	/*!
	* Write element to .mat file
	* Function name: writeMatElement
	* @param file : The file to be written to
	* @param type : The data type of the element
	* @param data : The data to be written
	* @param numBytes : The number of bytes of the data to be written
	* @return void
	* Function: This function is used to write data sub-elements of MAT files, with the core functionality being: small data is stored in a compressed format, while large data is stored in a standard format.
	*/
	void writeMatElement(std::ofstream& file, MatDataType type, const void* data, uint32_t numBytes);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<double>(const double* data, std::size_t size, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<int>(const int* data, std::size_t size, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<bin>(const bin* data, std::size_t size, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<std::complex<double>>(const std::complex<double>* data, std::size_t size, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<double>(const Vec<double>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<int>(const Vec<int>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<bin>(const Vec<bin>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<std::complex<double>>(const Vec<std::complex<double>>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<double>(const double* data, size_t rows, size_t cols, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<int>(const int* data, size_t rows, size_t cols, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<bin>(const bin* data, size_t rows, size_t cols, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<std::complex<double>>(const std::complex<double>* data, size_t rows, size_t cols, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<double>(const Mat<double>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<int>(const Mat<int>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<bin>(const Mat<bin>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT void saveData<std::complex<double>>(const Mat<std::complex<double>>& data, const std::string& fileName, int fileType, int writeMode, bool isNeedHeader);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<double> loadData<double>(const std::string& fileName, size_t count);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<int> loadData<int>(const std::string& fileName, size_t count);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<bin> loadData<bin>(const std::string& fileName, size_t count);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<std::complex<double>> loadData<std::complex<double>>(const std::string& fileName, size_t count);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<std::string> loadData<std::string>(const std::string& fileName, size_t count);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<double> loadData<double>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<int> loadData<int>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<bin> loadData<bin>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<std::complex<double>> loadData<std::complex<double>>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Vec<std::string> loadData<std::string>(const std::string& fileName);

	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Mat<double> loadDatMat<double>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Mat<int> loadDatMat<int>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Mat<bin> loadDatMat<bin>(const std::string& fileName);
	ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT Mat<std::complex<double>> loadDatMat<std::complex<double>>(const std::string& fileName);
}