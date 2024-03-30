// IDevice.cpp
//
#include "IDevice.hpp"


Byte IDevice::read(Word offset, bool debug) 
{
	if (offset - m_base < m_size)
		return m_memory[(Word)(offset - m_base)];
	return 0xCC;
}

void IDevice::write(Word offset, Byte data, bool debug) 
{
	if (offset - m_base < m_size)
		m_memory[(Word)(offset - m_base)] = data;
}

Word IDevice::read_word(Word offset, bool debug) 
{
	Word ret = (read(offset) << 8) | read(offset + 1);
	return ret;
}

void IDevice::write_word(Word offset, Word data, bool debug)
{
	Uint8 msb = (data >> 8) & 0xFF;
	Uint8 lsb = data & 0xff;
	write(offset, msb);
	write(offset + 1, lsb);
}

void IDevice::DisplayEnum(std::string sToken, Word ofs, std::string sComment)
{
	// lambda to convert integer to hex string
	auto hex = [](uint32_t n, uint8_t digits)
	{
		std::string s(digits, '0');
		for (int i = digits - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};	

	if (COMPILE_MEMORY_MAP)
	{
		std::string sN = "$";
		std::string sCTK = "  ; ";
		std::string sCMA = "  ";
		std::string sPAD = "";
		if (MEMORY_MAP_OUTPUT_CPP)
		{
			sN = "0x";
			sCTK = "// ";
			sCMA = ", ";
			sPAD = "    ";
		}
		if (sComment.empty())
			sCTK = "";
		// single line comment
		if (sToken.empty())
		{
			printf("        %s%s\n", sCTK.c_str(), sComment.c_str());
		}
		// normal line
		else
		{
			std::string sLabel = sToken;
			while (sToken.size() < 16)
				sToken += " ";
			if (MEMORY_MAP_OUTPUT_CPP)
				sToken += " = ";
			else
				sToken += "    equ   ";
			std::cout << sPAD << sToken << sN << hex(ofs, 4) << sCMA << sCTK << sComment << std::endl;	
		}
	}
}






// ROM /////////////////////////////////////

// only write to ROM when the debug flag is true
void ROM::write(Word offset, Byte data, bool debug) 
{
    if (debug)
        if (offset - m_base < m_size)
            m_memory[(Word)(offset - m_base)] = data;
}
