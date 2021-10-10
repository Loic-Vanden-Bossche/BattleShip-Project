
typedef struct
{
	int musicVolume;
	int effectsVolume;
	int selectedLanguage;
	int displayMode;
	int displayRes;
	int rotateKey;

} userSettings;

static int playerGrid[100] = {0};

static int IAGrid[100] = {0};

static userSettings settings;

userSettings getUserSettings(){

	return settings;
}

void setUserSettings(userSettings tempSettings){

	settings = tempSettings;
}


void setPlayerCase(int i, int value) {

	playerGrid[i] = value;
}

int getPlayerCase(int i) {

	return playerGrid[i];
}

void setIACase(int i, int value) {

	IAGrid[i] = value;
}

int getIACase(int i) {

	return IAGrid[i];
}
