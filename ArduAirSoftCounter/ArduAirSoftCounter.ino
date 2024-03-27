
#include <Adafruit_Debounce.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

Adafruit_Debounce btnMinus(4, LOW);
Adafruit_Debounce btnPlus(5, LOW);
Adafruit_Debounce btnEnter(6, LOW);

enum E_STATE {
	PreGameInit,
	PreGameSpawns,
	PreGameTimePerBlock,
	PreGameSpawnBlock,
	GameInProgress,
	Finish
};

E_STATE _state;

int maxCounter, counter = 0;
int lockCounter = 0;
int lockLeftSeconds,lockSeconds = 0;

int spawnLock = 0;
bool CountDeads = true;


void setup() {
	Serial.begin(115200);
	btnMinus.begin();
	btnPlus.begin();
	btnEnter.begin();

	lcd.init(); // initialize the lcd
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);            // move cursor the first row
	lcd.print("Airsoft life by:");          // print message at the first row
	lcd.setCursor(0, 1);            // move cursor to the second row
	lcd.print(" pluciorx@gmail.com"); // print message at the second row
	lcd.setCursor(0, 2);            // move cursor to the third row
	lcd.print("v0.1 dla AGT-Airsoft "); // print message at the second row
	lcd.setCursor(0, 3);            // move cursor to the third row
	lcd.print("Grudziadz Team"); // print message at the second row
	delay(5000);
	SetState(E_STATE::PreGameSpawns);
}


void loop() {
	

	switch (_state)
	{
	case PreGameInit:
	{
		 counter = 0;
		 lockCounter = 0;
		 lockLeftSeconds, lockSeconds = 0;
		 
		 SetState(E_STATE::PreGameSpawns);
		 btnMinus.update();
		 btnPlus.update();
		 btnEnter.update();
	}break;
	case PreGameSpawns: {
		

		lcd.setCursor(0, 0);            
		lcd.print("Spawns available:");       
		
		lcd.setCursor(0, 3);
		lcd.print("(-)   (+)    (Next)");
		while (!btnEnter.justPressed())
		{
			btnMinus.update();
			btnPlus.update();
			btnEnter.update();

			if (btnMinus.justPressed())
			{
				Serial.println("Decreasing counter;");
				if (counter>0) counter--;
			}
			if (btnPlus.justPressed()) {
				Serial.println("Increasing counter;");
				counter++;
			}
			lcd.setCursor(0, 1);            
			lcd.print("  ");
			lcd.setCursor(0, 1);
			lcd.print(counter);
			delay(10);
		}
		if (btnEnter.justPressed())
		{
			SetState(E_STATE::PreGameTimePerBlock);
			btnEnter.update();			
			delay(50);
		}
	

		//display menu to increase counter for team lifes
	}break;
	case PreGameTimePerBlock: {

		lcd.setCursor(0, 0);           
		lcd.print("Lock Time (seconds):");      

		lcd.setCursor(0, 3);
		lcd.print("(-)   (+)    (Next)");
		while (!btnEnter.justPressed())
		{
			btnMinus.update();
			btnPlus.update();
			btnEnter.update();

			if (btnMinus.justPressed())
			{
				Serial.println("Decreasing time;");
				if (lockSeconds > 0) lockSeconds--;
			}
			if (btnPlus.justPressed()) {
				Serial.println("Increasing time;");
				lockSeconds++;
			}
			lcd.setCursor(0, 1);
			lcd.print("  ");
			lcd.setCursor(0, 1);
			lcd.print(lockSeconds);
			delay(10);
		}
		if (btnEnter.justPressed())
		{			
			SetState(E_STATE::PreGameSpawnBlock);
			btnEnter.update();
			delay(50);
		}

		//display menu to increase counter for team lifes
	}break;
	case PreGameSpawnBlock: {

		lcd.setCursor(0, 0);            
		lcd.print("Spawns locked:");     

		lcd.setCursor(0, 3);
		lcd.print("(-)   (+)    (Start)");
		while (!btnEnter.justPressed())
		{
			btnMinus.update();
			btnPlus.update();
			btnEnter.update();

			if (btnMinus.justPressed())
			{
				Serial.println("Decreasing spawnLock;");
				if (spawnLock > 0) spawnLock--;
			}
			if (btnPlus.justPressed()) {
				Serial.println("Increasing spawnLock;");
				spawnLock++;
			}
			lcd.setCursor(0, 1);
			lcd.print("  ");
			lcd.setCursor(0, 1);
			lcd.print(spawnLock);
			delay(10);
		}
		if (btnEnter.justPressed())
		{
			CountDeads = counter == 0 ;
			maxCounter = counter;
			SetState(E_STATE::GameInProgress);
			btnEnter.update();
			delay(50);
		}


		//display menu to increase counter for team lifes
	}break;

	case GameInProgress:{
		
		
		lcd.setCursor(0, 0);            
		lcd.print("Game in Progress");          
		
		if (CountDeads)
		{
			lcd.setCursor(0, 1);
			lcd.print("Spawns:");          
			if (btnEnter.justPressed())
			{
				Serial.println("Dead count increased");
				counter++;
			}
			lcd.setCursor(9, 1);
			lcd.print("  ");
			lcd.setCursor(9, 1);
			lcd.print(counter);
			
		}
		else {
			lcd.setCursor(0, 1);
			lcd.print("Spawns left:");
			lcd.setCursor(13, 1);
			lcd.print("  ");
			lcd.setCursor(13, 1);
			lcd.print(counter);
			
			if (btnEnter.justPressed() && lockCounter < spawnLock)
			{				
				counter--;
				Serial.println("Spawns available decreased");
				
				lcd.setCursor(13, 1);
				lcd.print("  ");
				lcd.setCursor(13, 1);
				lcd.print(counter);

				if (counter == 0)
				{
					lcd.clear();
					lcd.setCursor(0, 1);
					lcd.print("! Game Finished !");
					lcd.setCursor(0, 2);
					lcd.print("Spawn's used:");
					lcd.setCursor(0, 3);
					lcd.print(maxCounter);
					while (1)
					{ }// loop until someone will restart the device
					
				}
				lockCounter++;
				Serial.println(counter);
				Serial.println(lockCounter);
				
			}
			if (lockCounter == spawnLock)
			{
				lockLeftSeconds = lockSeconds;
				unsigned long currentLoopTime = millis();
				
				// we have reached the lock threshold start counting and lock the decreare
				lcd.setCursor(0, 2);
				lcd.print("Lock (s):");         			
				while (lockLeftSeconds > 0)
				{
					lcd.setCursor(13, 2);
					lcd.print("  ");

					lcd.setCursor(13, 2);
					lcd.print(lockLeftSeconds--);					
					lcd.setCursor(0, 3);
					lcd.print("               ");
					lcd.setCursor(0, 3);
					lcd.print("!!! LOCKED !!!");
					delay(1000);
				}
				
				
				lockCounter = 0; 
			}
			else {
				//we are just counting the deads
				lcd.setCursor(0, 2);
				lcd.print("        ");         
				lcd.print("  ");
				lcd.setCursor(13, 2);
				lcd.print("   ");
				lcd.setCursor(0, 3);
				lcd.print("               ");
				lcd.setCursor(0, 3);
				lcd.print("!!! UNLOCKED !!!");

			}
			


		}

		lcd.setCursor(0, 3);
		
		btnEnter.update();
		delay(10);

	}break;
	case Finish: {
		//Show finish message !
	}break;
				
	}
	
}

void SetState(E_STATE newState)
{
	
	_state = _state != newState ? newState : _state;
	lcd.clear();
}