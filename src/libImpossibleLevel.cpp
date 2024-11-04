#include "libImpossibleLevel.hpp"

//Source: https://codereview.stackexchange.com/a/22907
//modified to convert to and then return an unsigned char vector instead of a signed one
static std::vector<unsigned char> ReadAllBytes(char const* filename)
{
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    if (pos == 0) {
        return std::vector<unsigned char>{};
    }

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    std::vector<unsigned char> returnVal(pos);
    for(int i = 0; i < result.size(); i++)
    {
        returnVal[i] = static_cast<unsigned char>(result[i]);
    }

    return returnVal;
}

int swapIntEndians(int value) 
{ 
 
    // This var holds the leftmost 8 
    // bits of the output. 
    int leftmost_byte; 
 
    // This holds the left middle 
    // 8 bits of the output 
    int left_middle_byle; 
 
    // This holds the right middle 
    // 8 bits of the output 
    int right_middle_byte; 
 
    // This holds the rightmost 
    // 8 bits of the output 
    int rightmost_byte; 
 
    // To store the result 
    // after conversion 
    int result; 
 
    // Get the rightmost 8 bits of the number 
    // by anding it 0x000000FF. since the last 
    // 8 bits are all ones, the result will be the 
    // rightmost 8 bits of the number. this will 
    // be converted into the leftmost 8 bits for the 
    // output (swapping) 
    leftmost_byte = (value & 0x000000FF) >> 0; 
 
    // Similarly, get the right middle and left 
    // middle 8 bits which will become 
    // the left_middle bits in the output 
    left_middle_byle = (value & 0x0000FF00) >> 8; 
 
    right_middle_byte = (value & 0x00FF0000) >> 16; 
 
    // Get the leftmost 8 bits which will be the 
    // rightmost 8 bits of the output 
    rightmost_byte = (value & 0xFF000000) >> 24; 
 
    // Left shift the 8 bits by 24 
    // so that it is shifted to the 
    // leftmost end 
    leftmost_byte <<= 24; 
 
    // Similarly, left shift by 16 
    // so that it is in the left_middle 
    // position. i.e, it starts at the 
    // 9th bit from the left and ends at the 
    // 16th bit from the left 
    left_middle_byle <<= 16; 
 
    right_middle_byte <<= 8; 
 
    // The rightmost bit stays as it is 
    // as it is in the correct position 
    rightmost_byte <<= 0; 
 
    // Result is the concatenation of all these values. 
    result = (leftmost_byte | left_middle_byle |
              right_middle_byte | rightmost_byte); 
 
    return result; 
} 

int swapShortEndians(short value) 
{ 
    short leftmost_byte; 

    short rightmost_byte; 

    short result; 
 
    // Get the rightmost 8 bits of the number 
    // by anding it 0x000000FF. since the last 
    // 8 bits are all ones, the result will be the 
    // rightmost 8 bits of the number. this will 
    // be converted into the leftmost 8 bits for the 
    // output (swapping) 
    leftmost_byte = (value & 0x000000FF) >> 0;
 
    // Get the leftmost 8 bits which will be the 
    // rightmost 8 bits of the output 
    rightmost_byte = (value & 0xFF000000) >> 8; 
 
    // Left shift the 8 bits by 24 
    // so that it is shifted to the 
    // leftmost end 
    leftmost_byte <<= 8; 
 
    // The rightmost bit stays as it is 
    // as it is in the correct position 
    rightmost_byte <<= 0; 
 
    // Result is the concatenation of all these values. 
    result = (leftmost_byte | rightmost_byte); 
 
    return result; 
} 

int readIntFromJava(std::vector<unsigned char> file, int startingOffset)
{
    unsigned int bit1, bit2, bit3, bit4;
    bit1 = static_cast<unsigned int>(file[startingOffset]);
    bit2 = static_cast<unsigned int>(file[startingOffset + 1]);
    bit3 = static_cast<unsigned int>(file[startingOffset + 2]);
    bit4 = static_cast<unsigned int>(file[startingOffset + 3]);

    bit1 = bit1 << 24;
    bit2 = bit2 << 16;
    bit3 = bit3 << 8;
    //bit 4 doesn't get shifted

    unsigned int resultU = bit1 | bit2 | bit3 | bit4;
    int result = static_cast<int>(resultU);
    return result;
}

short readShortFromJava(std::vector<unsigned char> file, int startingOffset)
{
    unsigned short bit1, bit2;
    bit1 = static_cast<unsigned int>(file[startingOffset]);
    bit2 = static_cast<unsigned int>(file[startingOffset + 1]);

    bit1 = bit1 << 8;
    //bit 2 doesn't get shifted

    unsigned short resultU = bit1 | bit2;
    short result = static_cast<short>(resultU);
    return result;
}

void writeJavaInt(std::ofstream& datafile, int sourceInt)
{
    //datafile << static_cast<unsigned char>(static_cast<unsigned int>(swapIntEndians(sourceInt)) & 0xff);
    datafile << __builtin_bswap32(sourceInt);
}

void writeJavaShort(std::ofstream& datafile, short sourceShort)
{
    //datafile << static_cast<unsigned char>(static_cast<unsigned short>(swapShortEndians(sourceShort)) & 0xff);
    datafile << __builtin_bswap16(sourceShort);

}

void writeOtherData(std::ofstream& datafile, unsigned char data)
{
    datafile << data;
}

levelObj::levelObj()
{
    std::cout << "ERROR: Must provide a filepath!";
}

levelObj::levelObj(char const* filename)
{
    this->loadDataFromFile(filename);
}

//push back command: vector.push_back(&type);
void levelObj::loadDataFromFile(char const* filepath)
{
    int currentByte = 0;
    std::vector<unsigned char> *level = new std::vector<unsigned char>; 
    std::vector<unsigned char> temp = ReadAllBytes(filepath);
    level = &temp;
    std::cout << "getting file format version" << std::endl;
    formatVer = readIntFromJava(*level, currentByte);
    currentByte += 4;
    std::cout << "Format version " << formatVer << std::endl;
    std::cout << "Checking if special graphics are enabled" << std::endl;
    customGraphicsEnabled = static_cast<bool>(level->at(currentByte));
    if(customGraphicsEnabled)
    {
        std::cout << "Custom graphics enabled" << std::endl;
    }
    currentByte += 1;
    std::cout << "Attempting to read block count" << std::endl;
    numBlocks = readShortFromJava(*level, currentByte);
    std::cout << "There are " << numBlocks << " blocks in the level" << std::endl;
    currentByte += 2;

    blockObj *currentBlock = new blockObj;

    for(int i = 0; i < numBlocks; i++)
    {
        currentBlock->objType = static_cast<int>(level->at(currentByte));
        std::cout << "The current block type is " << objNames[currentBlock->objType] << std::endl;
        currentByte++;

        currentBlock->xPos = readIntFromJava(*level, currentByte);
        std::cout << "The current block's xpos is " << currentBlock->xPos << std::endl;
        currentByte += 4;

        currentBlock->yPos = readIntFromJava(*level, currentByte);
        std::cout << "The current block's ypos is " << currentBlock->yPos << std::endl;
        currentByte += 4;

        currentBlock->indexInVec = i;
        std::cout << "This block can be found at index " << currentBlock->indexInVec << std::endl;
        blockObjs->push_back(*currentBlock);

        std::cout << "Loaded object successfully!" << std::endl;
    }

    std::cout << "Loaded " << blockObjs->size() << " object(s)!" << std::endl;
    delete currentBlock;

    endWallPos = readIntFromJava(*level, currentByte);
    std::cout << "End wall is located at X position " << endWallPos << std::endl;
    currentByte += 4;

    std::cout << "Attempting to read color change count" << std::endl;
    numBgSwitch = readIntFromJava(*level, currentByte);
    std::cout << "There are " << numBgSwitch << " color triggers in the level" << std::endl;
    currentByte += 4;

    bgCon *currentBg = new bgCon;

    for(int i = 0; i < numBgSwitch; i++)
    {
        currentBg->xPos = readIntFromJava(*level, currentByte);
        std::cout << "The current color trigger's xpos is " << currentBg->xPos << std::endl;
        currentByte += 4;

        currentByte++; //skip over custom graphics boolean

        currentBg->colorID = readIntFromJava(*level, currentByte);
        std::cout << "The current color type is " << colorNames[currentBg->colorID] << std::endl;
        currentByte += 4;

        currentBg->indexInVec = i;

        std::cout << "This color trigger can be found at index " << currentBg->indexInVec << std::endl;
        backgroundSwitches->push_back(*currentBg);

        std::cout << "Loaded color trigger successfully!" << std::endl;
    }

    std::cout << "Loaded " << backgroundSwitches->size() << " color trigger(s)!" << std::endl;
    delete currentBg;

    std::cout << "Attempting to read gravity change count" << std::endl;
    numGravitySwitch = readIntFromJava(*level, currentByte);
    std::cout << "There are " << numGravitySwitch << " gravity changes in the level" << std::endl;
    currentByte += 4;

    gravityChange *currentGrav = new gravityChange;

    for(int i = 0; i < numGravitySwitch; i++)
    {
        currentGrav->xPos = readIntFromJava(*level, currentByte);
        std::cout << "The current gravity trigger's xpos is " << currentGrav->xPos << std::endl;
        currentByte += 4;

        currentGrav->indexInVec = i;

        std::cout << "This gravity trigger can be found at index " << currentGrav->indexInVec << std::endl;
        gravitySwitches->push_back(*currentGrav);

        std::cout << "Loaded gravity trigger successfully!" << std::endl;
    }

    std::cout << "Loaded " << gravitySwitches->size() << " gravity trigger(s)!" << std::endl;
    delete currentGrav;

    std::cout << "Attempting to read falling block count" << std::endl;
    numFallingBlocks = readIntFromJava(*level, currentByte);
    std::cout << "There are " << numFallingBlocks << " falling blocks in the level" << std::endl;
    currentByte += 4;

    fallingBlocks *currentFalling = new fallingBlocks;

    for(int i = 0; i < numFallingBlocks; i++)
    {
        currentFalling->startX = readIntFromJava(*level, currentByte);
        std::cout << "The current falling block startX is " << currentFalling->startX << std::endl;
        currentByte += 4;

        currentFalling->endX = readIntFromJava(*level, currentByte);
        std::cout << "The current falling block endX is " << currentFalling->endX << std::endl;
        currentByte += 4;

        currentFalling->indexInVec = i;

        std::cout << "This falling block section can be found at index " << currentFalling->indexInVec << std::endl;
        fallingSections->push_back(*currentFalling);

        std::cout << "Loaded falling block section successfully!" << std::endl;
    }

    std::cout << "Loaded " << fallingSections->size() << " falling section(s)!" << std::endl;
    delete currentFalling;

    std::cout << "Attempting to read rising block count" << std::endl;
    numRisingBlocks = readIntFromJava(*level, currentByte);
    std::cout << "There are " << numRisingBlocks << " rising blocks in the level" << std::endl;
    currentByte += 4;

    risingBlocks *currentRising = new risingBlocks;

    for(int i = 0; i < numFallingBlocks; i++)
    {
        currentRising->startX = readIntFromJava(*level, currentByte);
        std::cout << "The current rising block startX is " << currentRising->startX << std::endl;
        currentByte += 4;

        currentRising->endX = readIntFromJava(*level, currentByte);
        std::cout << "The current rising block endX is " << currentRising->endX << std::endl;
        currentByte += 4;

        currentRising->indexInVec = i;

        std::cout << "This rising block section can be found at index " << currentRising->indexInVec << std::endl;
        risingSections->push_back(*currentRising);

        std::cout << "Loaded rising block section successfully!" << std::endl;
    }

    std::cout << "Loaded " << risingSections->size() << " rising section(s)!" << std::endl;
    delete currentRising;

    std::cout << "Loaded entire level!" << std::endl;
}

//very unfinished
void levelObj::writeDataToFile(char const* filepath)
{
    std::ofstream dataOut;
    dataOut.open(filepath, std::ios_base::binary | std::ios_base::out);
    writeJavaInt(dataOut, formatVer);
    writeOtherData(dataOut, customGraphicsEnabled);
    writeJavaShort(dataOut, numBlocks);
    blockObj temp;
    for(int i = 0; i < numBlocks; i++)
    {
        temp = getBlockAtIndex(i);
        writeOtherData(dataOut, temp.objType);
        writeJavaInt(dataOut, temp.xPos);
        writeJavaInt(dataOut, temp.yPos);
    }
    writeJavaInt(dataOut, endWallPos);
    writeJavaInt(dataOut, numBgSwitch);
    bgCon tempCon;
    for(int i = 0; i < numBgSwitch; i++)
    {
        tempCon = getBgConAtIndex(i);
        writeJavaInt(dataOut, tempCon.xPos);
        writeJavaInt(dataOut, tempCon.colorID);
    }
    writeJavaInt(dataOut, numGravitySwitch);
    gravityChange tempGrav;
    for(int i = 0; i < numGravitySwitch; i++)
    {
        tempGrav = getGravityAtIndex(i);
        writeJavaInt(dataOut, tempGrav.xPos);
    }
    writeJavaInt(dataOut, numRisingBlocks);
    risingBlocks tempRising;
    for(int i = 0; i < numRisingBlocks; i++)
    {
        tempRising = getRisingAtIndex(i);
        writeJavaInt(dataOut, tempRising.startX);
        writeJavaInt(dataOut, tempRising.endX);
    }
    writeJavaInt(dataOut, numFallingBlocks);
    fallingBlocks tempFalling;
    for(int i = 0; i < numFallingBlocks; i++)
    {
        tempFalling = getFallingAtIndex(i);
        writeJavaInt(dataOut, tempFalling.startX);
        writeJavaInt(dataOut, tempFalling.startX);
    }
}

int levelObj::getFormatVer()
{
    return formatVer;
}

blockObj levelObj::getBlockAtIndex(int index)
{
    return blockObjs->at(index);
}

bgCon levelObj::getBgConAtIndex(int index)
{
    return backgroundSwitches->at(index);

}

gravityChange levelObj::getGravityAtIndex(int index)
{
    return gravitySwitches->at(index);
}

risingBlocks levelObj::getRisingAtIndex(int index)
{
    return risingSections->at(index);
}

fallingBlocks levelObj::getFallingAtIndex(int index)
{
    return fallingSections->at(index);
}

int levelObj::getEndPos()
{
    return endWallPos;
}

int levelObj::getObjCount()
{
    return numBlocks;
}

int levelObj::getBgCount()
{
    return numBgSwitch;
}

int levelObj::getGravityCount()
{
    return numGravitySwitch;
}

int levelObj::getRisingCount()
{
    return numRisingBlocks;
}

int levelObj::getFallingCount()
{
    return numFallingBlocks;
}

void levelObj::addNewBlock(blockObj toAdd)
{
    blockObjs->push_back(toAdd);
}

void levelObj::addBgCon(bgCon toAdd)
{
    backgroundSwitches->push_back(toAdd);
}

void levelObj::addGravitySwitch(gravityChange toAdd)
{
    gravitySwitches->push_back(toAdd);
}

void levelObj::addRisingBlocks(risingBlocks toAdd)
{
    risingSections->push_back(toAdd);
}

void levelObj::addFallingBlocks(fallingBlocks toAdd)
{
    fallingSections->push_back(toAdd);
}

void levelObj::setEndPos(int endPos)
{
    endWallPos = endPos;
}

void levelObj::printAllInfo()
{

}
