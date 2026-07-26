#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

class VehicleVals
{
public:
	VehicleVals(const std::string& filePath);
	~VehicleVals() {
		// clear sections
		for (auto& s : sections)
			s.clearSection();
	}

	std::string checkError();

	std::string getSectionTitle(uint32_t sectionID ){
		if (sectionID >= sections.size()) return "ERROR: Out of bounds!";
		return sections.at(sectionID).getTitle();
	}
	uint32_t getTotalSectionCount() {
		return sections.size();
	}

	uint32_t getSectionItemCount(uint32_t SectionID) {
		return sections.at(SectionID).getItemCount();
	}

	bool getItemData(uint32_t section, uint32_t itemID, uint32_t& id, std::string& descr, std::string& data) {
		if (section >= sections.size()) return false;
		return sections.at(section).getItemData(itemID, id, descr, data);
	}

	int32_t getItemID(uint32_t section, uint32_t itemIndex) {
		return sections.at(section).getItemID(itemIndex);
	}

	bool checkIfDescrOnly(uint32_t section, uint32_t itemID) {
		return sections.at(section).checkIfDescrOnly(itemID);
	}

	bool updateValue(const std::string& data, uint32_t section, uint32_t itemID) {
		return sections.at(section).updateItemValue(data, itemID);
	}

	void writeBinaryFile(const std::string& filePath);
	void writeTextFile(const std::string& filePath);

private:

	struct BaseData {
		int32_t id;
		bool labelOnly = false;
		std::string description;

		virtual std::string getData() = 0;
		virtual void writeData(std::ofstream& file) = 0;
		virtual bool updateData(const std::string& newdata) = 0;
	};
	struct Fdata : BaseData {
		float data;

		std::string getData() {
			return std::to_string(data);
		}
		
		void writeData(std::ofstream& file) {
			if (!labelOnly)
				file.write((char*)&data, sizeof(data));
		}

		bool updateData(const std::string& newdata) {
			try {
				data = std::stof(newdata);
			}
			catch (...) {
				return false;
			}
			
			return true;
		}
	};

	struct Sdata : BaseData {
		std::string strValue;
		std::string getData() {
			return strValue;
		}
		void writeData(std::ofstream& file) {
			if (!labelOnly)
				file.write(strValue.c_str(), 16);
		}
		bool updateData(const std::string& newdata) {
			strValue.assign(newdata);
			return true;
		}
	};

	struct I32data : BaseData {
		int32_t data;

		std::string getData() {
			return std::to_string(data);
		}
		void writeData(std::ofstream& file) {
			if (!labelOnly)
				file.write((char*)&data, sizeof(data));
		}
		bool updateData(const std::string& newdata) {
			data = std::stoi(newdata);
			long buf;
			return true;
		}
	};

	struct I16data : BaseData {
		int16_t data;

		std::string getData() {
			return std::to_string(data);
		}
		void writeData(std::ofstream& file) {
			if (!labelOnly)
				file.write((char*)&data, sizeof(data));
		}
		bool updateData(const std::string& newdata) {
			data = std::stoi(newdata);
			return true;
		}
	};

	struct Bdata : BaseData {
		int8_t data;
		std::string getData() {
			return std::to_string((int32_t)data);
		}
		void writeData(std::ofstream& file) {
			if (!labelOnly)
				file.write((char*)&data, sizeof(data));
		}
		bool updateData(const std::string& newdata) {
			data = std::stoi(newdata);
			return true;
		}
	};

	class Section {
	public:
		Section(const std::string& title) : title(title) {}
		void clearSection() {
			for (auto& sec : dataValues) {
				delete sec;
			}
			dataValues.clear();
		}

		std::string getTitle() {
			return title;
		}
		uint32_t getItemCount() {
			return dataValues.size();
		}
		bool checkIfDescrOnly(uint32_t itemID) {
			return dataValues.at(itemID)->labelOnly;
		}
		bool getItemData(uint32_t itemID, uint32_t& id, std::string& descr, std::string& data) {
			if (itemID >= dataValues.size()) return false;

			id = dataValues.at(itemID)->id;
			descr = dataValues.at(itemID)->description;
			data = dataValues.at(itemID)->getData();
			return true;
		}

		int32_t getItemID(uint32_t itemID) {
			return dataValues.at(itemID)->id;
		}

		void writeItemData(std::ofstream& file, int32_t itemID) {
			dataValues.at(itemID)->writeData(file);
		}

		bool updateItemValue(const std::string& s, int32_t itemID) {
			return dataValues.at(itemID)->updateData(s);
		}
		// add single label without values
		void Add(const std::string& description) {
			Sdata* sdata = new Sdata();
			sdata->description = description;
			sdata->labelOnly = true;

			dataValues.push_back(sdata);
		}
		// add float data
		void Add(const std::string& description, float& f) {
			Fdata* fdata = new Fdata();
			fdata->description = description;
			fdata->description.push_back(':');
			fdata->data = f;
			fdata->id = ++totalCount;

			dataValues.push_back(fdata);
		}

		// add integer data
		void Add(const std::string& description, int32_t i32) {
			I32data* idata = new I32data();
			idata->description = description;
			idata->description.push_back(':');
			idata->data = i32;
			idata->id = ++totalCount;

			dataValues.push_back(idata);
		}

		// add short data
		void Add(const std::string& description, int16_t i16) {
			I16data* idata = new I16data();
			idata->description = description;
			idata->description.push_back(':');
			idata->data = i16;
			idata->id = ++totalCount;

			dataValues.push_back(idata);
		}

		// add single byte data
		void Add(const std::string& description, int8_t b) {
			Bdata* bdata = new Bdata();
			bdata->description = description;
			bdata->description.push_back(':');
			bdata->data = b;
			bdata->id = ++totalCount;

			dataValues.push_back(bdata);
		}

		// add strings, usually limited to 15 bytes
		void Add(const std::string& description, char* str) {
			Sdata* sdata = new Sdata();
			sdata->description.assign(description);
			sdata->description.push_back(':');
			sdata->strValue.assign(str);
			sdata->id = ++totalCount;

			dataValues.push_back(sdata);
		}


	private:

		int32_t totalCount = 0;
		const std::string title;

		std::vector<BaseData*>dataValues;

	};
	
	static const uint16_t generalDataSize = 18;
	static const uint16_t gearsDataSize = 50;
	static const uint16_t generalPhysicsSize = 19;
	static const uint8_t wheelPairCount = 12;
	static const uint16_t unknownSection1Size = 12;
	static const uint16_t unknownSection2Size =  11 ;
	static const uint16_t unknownSection3Size = 50;
	static const uint16_t unknownSection4Size = 61;
	static const uint16_t unknownSection5Size = 56;


	std::string fileName;
	std::string lastError;

	std::vector<Section>sections;

	static uint32_t fillUp(std::ofstream& file, const std::string& str) {
		int16_t strDiff = 46 - str.length();
		if (strDiff < 0) return 0;
		for (uint8_t i = 1; i < strDiff; i++)
			file << ' ';
		return strDiff;
	}

};

