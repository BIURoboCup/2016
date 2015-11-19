#ifndef IMAGEPROCESS_H_
#define	IMAGEPROCESS_H_

#include "opencv2/opencv.hpp"
#include "ColorsValues.h"
#include "Objects/BallCandidatesList.h"
#include "IncludesFile.h"

#define BLUE_STR	"BLUE"
#define YELLOW_STR	"YELLOW"
#define ORANGE_STR	"ORANGE"
#define GREEN_STR	"GREEN"
#define WHITE_STR	"WHITE"
#define BLACK_STR	"BLACK"

#define MAX_TILT_FOR_GREEN_THRESHOLD_OFFSET (-5)
#define GREEN_TILT_THRESHOLD_OFFSET			7

extern int16_t m_greenMinusRedThreshold;
extern int16_t m_greenMinusBlueThreshold;

extern int16_t m_blueMinusRedThreshold;
extern int16_t m_blueMinusGreenThreshold;

extern uint8_t m_orangeMinHue;
extern uint8_t m_orangeMaxHue;
extern uint8_t m_orangeMinSat;
extern uint8_t m_orangeMinVal;

extern uint8_t m_yellowMinHue;
extern uint8_t m_yellowMaxHue;
extern uint8_t m_yellowMinSat;
extern uint8_t m_yellowMinVal;

extern uint8_t m_whiteMaxSat;
extern uint8_t m_whitewMinVal;

extern uint8_t m_blackMaxVal;

class ImageProcess {

private:

public:

	ImageProcess();

	void ImageHSVSegmentation(IplImage* hsvImage, IplImage* rgbImage);
	void RGBFilter(uchar* rgbImagePtr, uchar* hsvImagePtr);
	~ImageProcess();

	void LoadIniSettings(minIni* iniFile);
	void LoadColorSettings(minIni* iniFile, const std::string &section);

	void LoadDefaultColorSettings();
};

#endif	/* IMAGEPROCESS_H_ */

