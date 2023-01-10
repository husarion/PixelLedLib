/**
 * @file PixelLedLib.cpp
 * @author Maciej Kurcius
 * @brief 
 * @version 0.1
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <PixelLedLib.h>

/**
 * @brief Construct a new Pixel Led Class:: Pixel Led Class object
 * 
 * 
 */
PixelLedClass::PixelLedClass(){
    ;
}


/**
 * @brief Construct a new Pixel Led Class:: Pixel Led Class object
 * 
 * @param StripLength_ - quantity of physical leds in strip
 * @param Instance_ - instance identifier of leds strip. If you want use a few strip with different SPI and different init actions. 
 */
PixelLedClass::PixelLedClass(uint8_t StripLength_, uint8_t Instance_){
    StripLength = StripLength_;
    Red = new uint8_t[StripLength_];
    Green = new uint8_t[StripLength_];
    Blue = new uint8_t[StripLength_];
    Brightness = new uint8_t [StripLength_];
    VirtualPtr = new bool [StripLength_];
    Instance = Instance_;
    for(int i = 0; i < this->StripLength; i++)
        VirtualPtr[i] = false;
    PixelStripMapInit();
}

/**
 * @brief Construct a new Pixel Led Class:: Pixel Led Class object
 * 
 * @param StripLength_ - quantity of physical leds in strip
 * @param VirtualLedLength_ - quantity of virtual leds in strip
 * @param Instance_ - instance identifier of leds strip. If you want use a few strip with different SPI and different init actions. 
 */
PixelLedClass::PixelLedClass(uint8_t StripLength_, uint8_t VirtualLedLength_, uint8_t Instance_){
    StripLength = StripLength_ + VirtualLedLength_;
    Red = new uint8_t[StripLength_ + VirtualLedLength_];
    Green = new uint8_t[StripLength_ + VirtualLedLength_];
    Blue = new uint8_t[StripLength_ + VirtualLedLength_];
    Brightness = new uint8_t [StripLength_ + VirtualLedLength_];
    VirtualPtr = new bool [StripLength_ + VirtualLedLength_];
    Instance = Instance_;
    for(int i = 0; i < this->StripLength; i++)
        VirtualPtr[i] = false;
    PixelStripMapInit();
}

/**
 * @brief Destroy the Pixel Led Class:: Pixel Led Class object
 * 
 */
PixelLedClass::~PixelLedClass(){
    ;
}

/**
 * @brief Function for init Pixel LED strip
 * 
 * @return true - if error
 * @return false - if ok
 */
bool PixelLedClass::Init(){
    if(PixelSpiInit(this) || PixelInitActions(this))
        return true;
    return false;
}

/**
 * @brief Function for sending start frame bytes
 * 
 */
void PixelLedClass::SendStartFrame(){
    for(int i = 0; i < 4; i++)
        PixelSpiTransferData(this, 0x00);
}

/**
 * @brief Function for sending stop frame bytes. 
 * 
 * 
 */
void PixelLedClass::SendStopFrame(){
    for(int i = 0; i < 4; i++)
        PixelSpiTransferData(this, 0xFF);
}

/**
 * @brief Function for send all data to pixel led strip.
 * 
 */
void PixelLedClass::SendBuffersData(){
    this->SendStartFrame();
    for(int i = 0; i < StripLength; i++){
        if(VirtualPtr[i] == false){
            PixelSpiTransferData(this, Brightness[i] | 0xE0);
            PixelSpiTransferData(this, Blue[i]);
            PixelSpiTransferData(this, Green[i]);
            PixelSpiTransferData(this, Red[i]);
        }
    }
    this->SendStopFrame();
}

/**
 * @brief Function return strip length.
 * 
 * @return uint8_t 
 */
uint8_t PixelLedClass::GetStripLength(void){
    return StripLength;
}

/**
 * @brief Function for set the same brightness for all leds in strip (global)
 * 
 * @param Brightness_ 
 */
void PixelLedClass::SetStripBrightness(uint8_t Brightness_){
    for(int i = 0; i < StripLength; i++){
        Brightness[i] = Brightness_;
    }
    this->SendBuffersData();
}

/**
 * @brief Function for set the same colour for all leds in strip (global)
 * 
 * @param Red_ 
 * @param Green_ 
 * @param Blue_ 
 */
void PixelLedClass::SetStripColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_){
    for(int i = 0; i < StripLength; i++){
        Red[i] = Red_;
        Green[i] = Green_;
        Blue[i] = Blue_;
    }
    this->SendBuffersData();
}

/**
 * @brief Function for set the same colour and brightness for all leds in strip (global)
 * 
 * @param Red_ 
 * @param Green_ 
 * @param Blue_ 
 * @param Brightness_ 
 */
void PixelLedClass::SetStripColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_, uint8_t Brightness_){
    for(int i = 0; i < StripLength; i++){
        Red[i] = Red_;
        Green[i] = Green_;
        Blue[i] = Blue_;
        Brightness[i] = Brightness_;
    }
    this->SendBuffersData();
}

/**
 * @brief Function for set colour in n-th led in pixel strip
 * 
 * @param Led_ 
 * @param Red_ 
 * @param Green_ 
 * @param Blue_ 
 * @return true - if wrong data input
 * @return false 
 */
bool PixelLedClass::SetNthLed(uint8_t Led_, uint8_t Red_, uint8_t Green_, uint8_t Blue_){
    if(Led_ >= StripLength)
        return true;
    Red[PixelStripMap[Led_]] = Red_;
    Green[PixelStripMap[Led_]] = Green_;
    Blue[PixelStripMap[Led_]] = Blue_;
    this->SendBuffersData();
    return false;
}

/**
 * @brief Function for set colour and brightness in n-th led in pixel strip
 * 
 * @param Led_ 
 * @param Red_ 
 * @param Green_ 
 * @param Blue_ 
 * @param Brightness_ 
 * @return true - if wrong data input
 * @return false 
 */
bool PixelLedClass::SetNthLed(uint8_t Led_, uint8_t Red_, uint8_t Green_, uint8_t Blue_, uint8_t Brightness_){
    if(Led_ >= StripLength || Led_ < 0)
        return true;
    Red[PixelStripMap[Led_]] = Red_;
    Green[PixelStripMap[Led_]] = Green_;
    Blue[PixelStripMap[Led_]] = Blue_;
    Brightness[PixelStripMap[Led_]] = Brightness_;
    this->SendBuffersData();
    return false;
}

/**
 * @brief Function for set colour and brightness of n-th led in strip without sending data
 * 
 * @param Led_ 
 * @param Red_ 
 * @param Green_ 
 * @param Blue_ 
 * @param Brightness_ 
 * @return true - if wrong data input
 * @return false 
 */
bool PixelLedClass::SetNthLedBuffer(uint8_t Led_, uint8_t Red_, uint8_t Green_, uint8_t Blue_, uint8_t Brightness_){
    if(Led_ >= StripLength || Led_ < 0)
        return true;
    Red[PixelStripMap[Led_]] = Red_;
    Green[PixelStripMap[Led_]] = Green_;
    Blue[PixelStripMap[Led_]] = Blue_;
    Brightness[PixelStripMap[Led_]] = Brightness_;
    return false;
}

/**
 * @brief Function for set brightness of n-th led in strip without sending data
 * 
 * @param Led_ 
 * @param Brightness_ 
 */
void PixelLedClass::SetNthBrightness(uint8_t Led_, uint8_t Brightness_){
    Brightness[PixelStripMap[Led_]] = Brightness_;
    this->SendBuffersData();
}

/**
 * @brief Function for initialize pixel map. 
 * 
 */
void PixelLedClass::PixelStripMapInit(void){
    for(int i = 0; i < StripLength; i++)
        PixelStripMap[i] = i;
}

/**
 * @brief Function for remap pixel numbers. Use this function if the order of mechanical and electrical connections is different.
 * 
 * @param MapIndex_ 
 * @param LedIndex_ 
 * @return true 
 * @return false 
 */
bool PixelLedClass::PixelStripMapRemap(uint8_t MapIndex_, uint8_t LedIndex_){
    if(MapIndex_ >= StripLength || LedIndex_ >= StripLength)
        return true;
    PixelStripMap[MapIndex_] = LedIndex_;
    return false;
}

/**
 * @brief 
 * 
 * @param LedIndex1_ 
 * @param LedIndex2_ 
 * @return true 
 * @return false 
 */
bool PixelLedClass::PixelStripMapSwap(uint8_t LedIndex1_, uint8_t LedIndex2_){
    if(LedIndex1_ >= StripLength || LedIndex2_ >= StripLength)
        return true;
    PixelStripMap[LedIndex1_] = LedIndex2_;
    PixelStripMap[LedIndex2_] = LedIndex1_;
    return false;
}

/**
 * @brief 
 * 
 * @param Led_ 
 * @return true 
 * @return false 
 */
bool PixelLedClass::SetNthLedAsVirtual(uint8_t Led_){
    if(Led_ > this->StripLength)
        return true;
    this->VirtualPtr[Led_] = true;
    return false;
}

/**
 * @brief 
 * 
 * @param Leds_ 
 * @param Size_ 
 * @return true 
 * @return false 
 */
bool PixelLedClass::SetLedsAsVirtual(uint8_t* Leds_, uint8_t Size_){
    for(int i = 0; i < Size_; i++){
        if(Leds_[i] > this -> StripLength)
            return true;
        this->VirtualPtr[Leds_[i]] = true;
    }
    return false;
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t PixelLedClass::GetInstance(void){
    return this->Instance;
}

/**
 * @brief Construct a new Pixel Strip Subset Class:: Pixel Strip Subset Class object
 * 
 */
PixelStripSubsetClass::PixelStripSubsetClass(){
    ;
}

/**
 * @brief Construct a new Pixel Strip Subset Class:: Pixel Strip Subset Class object
 * 
 * @param PixelStrip_ 
 */
PixelStripSubsetClass::PixelStripSubsetClass(PixelLedClass *PixelStrip_){
    PixelStrip = PixelStrip_;
}

/**
 * @brief Construct a new Pixel Strip Subset Class:: Pixel Strip Subset Class object
 * 
 * @param PixelStrip_ 
 * @param FirstLed_ - the number of first led of subset in pixels strip. Give the number according to mechanical position.
 * @param LastLed_ - the number of last led of subset in pixels strip. Give the number according to mechanical position.
 */
PixelStripSubsetClass::PixelStripSubsetClass(PixelLedClass *PixelStrip_, uint8_t FirstLed_, uint8_t LastLed_){
    PixelStrip = PixelStrip_;
    if(FirstLed_ >= PixelStrip->GetStripLength() || LastLed_ >= PixelStrip->GetStripLength()){
        ErrorFlag = true;
        return;
    }
    Length = abs(LastLed_ - FirstLed_)+1;
    for(int i = 0; i < Length; i++){
        if(FirstLed_ <= LastLed_)
            SubsetStripMap[i] = FirstLed_ + i;
        else
            SubsetStripMap[i] = FirstLed_ - i;
    }
}

/**
 * @brief Destroy the Pixel Strip Subset Class:: Pixel Strip Subset Class object
 * 
 */
PixelStripSubsetClass::~PixelStripSubsetClass(){
    ;
}

bool PixelStripSubsetClass::SetSubset(uint8_t FirstLed_, uint8_t LastLed_){
    return false;
}

bool PixelStripSubsetClass::CheckErr(void){
    return ErrorFlag;
}


void PixelStripSubsetClass::StripMapFlip(void){
    std::map<uint8_t, uint8_t> temp_map;
    for(int i = 0; i < Length; i++)
        temp_map[i] = SubsetStripMap[(Length-1) - i];
}


void PixelStripSubsetClass::SetColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_){
    uint8_t temp=0;
    for(int i=0; i < Length; i++){
        temp = SubsetStripMap[i];
        this->PixelStrip->SetNthLed(SubsetStripMap[i], Red_, Green_, Blue_);
    }
}
void PixelStripSubsetClass::SetColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_, uint8_t Brightness_){
    for(int i=0; i < Length; i++){
        this->PixelStrip->SetNthLed(SubsetStripMap[i], Red_, Green_, Blue_, Brightness_);
    }
}
void PixelStripSubsetClass::SetBrightness(uint8_t Brightness_){
    ;
}
void PixelStripSubsetClass::SetNthLedColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_){
    ;
}
void PixelStripSubsetClass::SetNthLedColour(uint8_t Red_, uint8_t Green_, uint8_t Blue_, uint8_t Brightness_){

}
void PixelStripSubsetClass::SetNthLedBrightness(uint8_t Brightness_){
    ;
}


