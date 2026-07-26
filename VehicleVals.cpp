#include "VehicleVals.h"

VehicleVals::VehicleVals(const std::string& filePath) {

	fileName = filePath.substr(0, filePath.length() - 4);

    std::ifstream file(filePath, std::ios::binary);

	if (!file.good()) {
		lastError = "File not found!\n";
		return;
	}

    // check file size
    file.seekg(0, file.end);
    const std::streamoff fileSize = file.tellg();

    // valid files are always 4687 bytes big, its a fixed none dynamic size.
    if (fileSize != 4687) {
        file.close();
        lastError = "Bad format, can't interpret data!\n";
        return;
    }

    // reset reader
    file.seekg(0, file.beg);
    
    // buffers for reader
    float fbuf;
    int8_t i8buf;
    int16_t i16buf;
    int32_t i32buf;
    char strBuf[17];
    // make sure it ends with 0. sometimes strings use full length
    strBuf[16] = 0;
   

	// read first section, general data
    const char* values1[generalDataSize]{
        "Weight",
        "Power",
        "GearSpeedFactor",
        "RedLineRPM",
        "maximum rpm",
        "idling rpm",
        "Max Power at idling RPM",
        "Min Power at redline RPM",
        "PowerBandKwMax",
        "PowerBandRPMlo",
        "PowerBandRPMhi",
        "Hit Points",
        "Motor time",
        "SkidFactor",
        "Shift time",
        "Motor speed",
        "Down Force Front",
        "Down Force Rear"
    };

    sections.push_back(Section("General Data"));
    for (int32_t i = 0; i < generalDataSize; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(values1[i], fbuf);
    }

    // GEARS

    const char* gearVals[gearsDataSize]{
          "GEAR 1 ON",
          "GEARS",
          "Reverse gear",
          "shift up",
          "shift down",
          "GEAR 1",
          "shift up",
          "shift down",
          "GEAR 2",
          "shift up",
          "shift down",
          "GEAR 3",
          "shift up",
          "shift down",
          "GEAR 4",
          "shift up",
          "shift down",
          "GEAR 5",
          "shift up",
          "shift down",
          "GEAR 6",
          "shift up",
          "shift down",
          "GEAR 7",
          "shift up",
          "shift down",
          "GEAR 8",
          "shift up",
          "shift down",
          "GEAR 9",
          "shift up",
          "shift down",
          "GEAR 10",
          "shift up",
          "shift down",
          "GEAR 11",
          "shift up",
          "shift down",
          "GEAR 12",
          "shift up",
          "shift down",
          "GEAR 13",
          "shift up",
          "shift down",
          "GEAR 14",
          "shift up",
          "shift down",
          "GEAR 15",
          "shift up",
          "shift down"
    };

    sections.push_back(Section("Gear Data"));
    for (int32_t i = 0; i < gearsDataSize; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(gearVals[i], fbuf);
    }

    const char* vehPhysVals[generalPhysicsSize]{
        "unknown float 0.1-1",
        "Air Reistance Factor",
        "Front down multiplier",
        "Front down",
        "Rear down multiplier",
        "Rear down",
        "Front tires max angle (25° to 90°",
        "Front tires Skid Factor (0° to 90°)",
        "Front tires offset angle (0° to #7°)",
        "main Front wheel speed",
        "main Front wheel comparision speed",
        "sec Front wheel speed",
        "sec Front wheel comparision speed",
        "Axis Z rotate",
        "Center mass",
        "Axis X rotate",
        "Gravitational acceleration",
        "Fuel tank volume",
        "Fuel consumption"
    };

    sections.push_back(Section("General Physic Data"));
    for (int32_t i = 0; i < generalPhysicsSize; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(vehPhysVals[i], fbuf);
    }

    const char* advPhysVals[]{
        "Suspension strength",
        "Suspension fwd (height)",
        "Shock-absorber fwd (harshness)",
        "Shock-absorber rear (harshness)",
        "Toe-in/toe-out angle (-10° to 10°)",
        "Camber angle (-10° to 10°)",
        "Caster angle",
        "Axle height offset",
        "Type of Wheels",
        "Wheel traction",
        "Weight on axle",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "Steering ability in %",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown end",
    };

    sections.push_back(Section("Wheel Data"));
    for (int32_t i = 0; i < wheelPairCount; i++) {
        std::string grpDescr("Wheel pair ");
        grpDescr.append(std::to_string(i + 1));
        sections.back().Add(grpDescr);
        for (int32_t s = 0; s < 8; s++) {
            file.read((char*)&fbuf, sizeof(fbuf));
            sections.back().Add(advPhysVals[s], fbuf);
        }
        file.read((char*)&i32buf, sizeof(i32buf));
        sections.back().Add("unknown Integer", i32buf);
        for (int32_t s = 0; s < 13; s++) {
            file.read((char*)&fbuf, sizeof(fbuf));
            sections.back().Add(advPhysVals[s + 9], fbuf);
        }
        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add("Drive axle enabled (true/false)", i8buf);
        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add("Steering enabled (true/false)", i8buf);
        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add("Handbrake enabled (true/false)", i8buf);

        const char* advPhysVals2[]{
            "Brake left ability in %",
            "Brake right ability in %",
            "SkidMark intensity: (1=highest / 90=lowest)",
            "SkidSound intensity: (1=highest / 50=lowest)",
            "Grip loss factor", //(can't be lower than value "27 of SkidMark intensity" and only to 90)
        };
        for (int32_t s = 0; s < 5; s++) {
            file.read((char*)&fbuf, sizeof(fbuf));
            sections.back().Add(advPhysVals2[s], fbuf);
        }

        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add("Dust trails enabled (yes=1 / no=0)", i8buf);
    }

    sections.push_back(Section("Unknown Section 1"));
    for (int32_t i = 0; i < 7; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }
    
    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte 1", i8buf);

    for (int32_t i = 0; i < 2; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read((char*)&i16buf, sizeof(i16buf));
    sections.back().Add("unknown short", i16buf);

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("unknown", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("Lock visual steering (True/False)", i8buf);

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("unknown", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("Roof (true=1, no=0)", i8buf);

    for (int32_t i = 0; i < 2; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read((char*)&i32buf, sizeof(i32buf));
    sections.back().Add("unknown Integer", i32buf);

    for (int32_t i = 0; i < 8; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte 3", i8buf);


    sections.push_back(Section("Unknown Section 2"));
    for (int32_t i = 0; i < unknownSection3Size; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read(strBuf, 16);
    sections.back().Add("Window image reference", strBuf);
    file.read(strBuf, 16);
    sections.back().Add("Window alpha image reference", strBuf);


    sections.push_back(Section("Unknown Section 3"));
    for (int32_t i = 0; i < unknownSection4Size; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte", i8buf);

    for (uint8_t x = 0; x < 4; x++) {
        std::string descr("generic sound reference ");
        descr.append(std::to_string(x+1));
        file.read(strBuf, 16);
        sections.back().Add(descr, strBuf);
    }

    sections.push_back(Section("Unknown Section 4"));
    sections.back().Add("Group 1");
    for (int32_t i = 0; i < unknownSection5Size; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }
    
    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("unknown", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte", i8buf);

    file.read(strBuf, 16);
    sections.back().Add("generic sound reference", strBuf);

    sections.back().Add("Group 2");
    for (int32_t i = 0; i < unknownSection5Size; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("unknown", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte", i8buf);

    file.read(strBuf, 16);
    sections.back().Add("generic sound reference", strBuf);

    sections.back().Add("Group 3");
    for (int32_t i = 0; i < unknownSection5Size; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown", fbuf);
    }

    file.read(strBuf, 16);
    sections.back().Add("generic sound reference", strBuf);

    // FINAL SECTION !
    sections.push_back(Section("Advanced Sound Data"));
    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("interior val 1?", fbuf);
    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("interior val 2?", fbuf);

    file.read(strBuf, 16);
    sections.back().Add("interior text 1", strBuf);

    file.read(strBuf, 16);
    sections.back().Add("interior text 2", strBuf);

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("horn value?", fbuf);

    const char* misc[]{
        "horn ref",
        "unknown string ref",
        "hand ref",
        "gear 1 ref",
        "gear 2 ref",
        "slip ref",
        "loop ref",
        "hit 1 ref",
        "unknown ref",
        "glass ref"
    };

    for (uint8_t i = 0; i < 10; i++) {
        file.read(strBuf, 16);
        sections.back().Add(misc[i], strBuf);
    }

    for (uint8_t i = 0; i < 4; i++) {
        file.read(strBuf, 16);
        sections.back().Add("crash 1 ref", strBuf);

        file.read(strBuf, 16);
        sections.back().Add("crash 2 ref", strBuf);

        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("crash val", fbuf);

        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add("crash byte", i8buf);
    }

    for (uint8_t i = 0; i < 6; i++) {
        std::string name("base engine sound ref ");
        name.append(std::to_string(i + 1));

        file.read(strBuf, 16);
        sections.back().Add(name, strBuf);
    }

    for (uint8_t i = 0; i < 4; i++) {
        std::string name("base engine sound float ");
        name.append(std::to_string(i + 1));
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(name, fbuf);
    }

    for (uint8_t i = 0; i < 3; i++) {
        std::string name("forward rpm engine sound ref ");
        name.append(std::to_string(i + 1));
        file.read(strBuf, 16);
        sections.back().Add(name, strBuf);

        for (uint8_t j = 0; j < 8; j++) {
            name.assign("forward rpm engine sound vals ");
            name.append(std::to_string(j + 1));
            file.read((char*)&fbuf, sizeof(fbuf));
            sections.back().Add(name, fbuf);
        }

        name.assign("rpm engine unknown Byte ");
        name.append(std::to_string(i + 1));
        file.read((char*)&i8buf, sizeof(i8buf));
        sections.back().Add(name, i8buf);
    }

    file.read(strBuf, 16);
    sections.back().Add("forward rpm sound final ref", strBuf);

    for (uint8_t i = 0; i < 8; i++) {
        std::string name("final rpm engine sound vals ");
        name.append(std::to_string(i + 1));
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(name, fbuf);
    }

    for (uint8_t i = 0; i < 12; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown data", fbuf);
    }

    file.read((char*)&i16buf, sizeof(i16buf));
    sections.back().Add("unknown Short", i16buf);

    file.read(strBuf, 16);
    sections.back().Add("rpm x reference", strBuf);

    for (uint8_t i = 0; i < 8; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("rpm x value " + std::to_string(i+1), fbuf);
    }

    for (uint8_t i = 0; i < 49; i++) {
        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add("unknown float", fbuf);
    }

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("unknown Byte", i8buf);

    for (uint8_t j = 0; j < 5; j++) {
        std::string name("general sound ref");
        name.append(std::to_string(j + 1));
        file.read(strBuf, 16);
        sections.back().Add("general sound ref", strBuf);

        for (uint8_t i = 0; i < 8; i++) {
            std::string name("general sound value ");
            name.append(std::to_string(i + 1));
            file.read((char*)&fbuf, sizeof(fbuf));
            sections.back().Add(name, fbuf);
        }
    }

    file.read(strBuf, 16);
    sections.back().Add("rpm idle reference", strBuf);

    for (uint8_t i = 0; i < 6; i++) {
        std::string name("rpm idle value ");
        name.append(std::to_string(i + 1));

        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(name, fbuf);
    }

    file.read(strBuf, 16);
    sections.back().Add("A sound ref", strBuf);

    for (uint8_t i = 0; i < 9; i++) {
        std::string name("A sound ref ");
        name.append(std::to_string(i + 1));

        file.read((char*)&fbuf, sizeof(fbuf));
        sections.back().Add(name, fbuf);
    }

    const char* strs[]{
        "horn ref",
        "unknown ref",
        "hand ref",
        "hear ref 1",
        "hear ref 2",
        "skid ref",
        "freak ref",
        "pep ref",
        "pneu ref",
        "car glass general ref",
        "car glass ref a",
        "car glass ref b",
        "car glass ref c",
        "car glass ref d",
        "car glass ref B1",
        "car glass ref B2",
        "car glass ref C1",
        "car glass ref C2",
        "door 1 ref",
        "door 2 ref"
    };

    for (uint8_t i = 0; i < 12; i++) {
        file.read(strBuf, 16);
        sections.back().Add(strs[i], strBuf);
    }

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("car glass value 1", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("car glass Byte value 1", i8buf);

    for (uint8_t i = 12; i < 14; i++) {
        file.read(strBuf, 16);
        sections.back().Add(strs[i], strBuf);
    }

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("crash value 1", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("crash Byte value 1", i8buf);

    for (uint8_t i = 14; i < 16; i++) {
        file.read(strBuf, 16);
        sections.back().Add(strs[i], strBuf);
    }

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("crash value 2", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("crash Byte value 2", i8buf);

    for (uint8_t i = 16; i < 18; i++) {
        file.read(strBuf, 16);
        sections.back().Add(strs[i], strBuf);
    }

    file.read((char*)&fbuf, sizeof(fbuf));
    sections.back().Add("crash value 3", fbuf);

    file.read((char*)&i8buf, sizeof(i8buf));
    sections.back().Add("crash Byte value 3", i8buf);

    for (uint8_t i = 18; i < 20; i++) {
        file.read(strBuf, 16);
        sections.back().Add(strs[i], strBuf);
    }

	file.close();
}

void VehicleVals::writeTextFile(const std::string& filePath) {
    std::ofstream newFile(filePath, std::ios::binary|std::ios::trunc);

    newFile << "Vehicle data from table " << fileName << ", version 1.03" << '\n';

    for (auto& sec : sections) {
        newFile << '\n' << sec.getTitle() << ":\n";
        const uint32_t items = sec.getItemCount();
        for (uint32_t i = 0; i < items; i++) {

            uint32_t itemID;
            std::string descr;
            std::string dataVal;
            sec.getItemData(i, itemID, descr, dataVal);
            if (sec.checkIfDescrOnly(i))
                newFile << descr;
            else {
                newFile << itemID;
                newFile << '\t';
                newFile << descr;
                fillUp(newFile, descr);
                newFile << dataVal;
            }
            newFile << '\n';
        }
    }

    newFile.close();
}

void VehicleVals::writeBinaryFile(const std::string& filePath) {
    std::ofstream newFile(filePath, std::ios::binary|std::ios::trunc);
    for (auto& sec : sections) {
        for (uint32_t i = 0; i < sec.getItemCount(); i++) {
            sec.writeItemData(newFile, i);
        }
    }
    // this data junk is always in the end for some reason
    int t = -842150451; // represents CD CD CD CD
    newFile.write((char*)&t, 4);
    newFile.close();
}

std::string VehicleVals::checkError() {
    return lastError;
}
