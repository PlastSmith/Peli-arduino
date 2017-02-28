#include <LiquidCrystal.h> // LCD Näytön kirjasto
#include <Ethernet.h>
#include <SPI.h>

// LiquidCrystal(rs, enable, d4, d5, d6, d7) 
LiquidCrystal lcd(8, 7, 5, 4, 3, 2);

int ledPin = 6;
char luetaanNappain();
int AD_conversio(byte ch);
void ask_IP(void);
//void lahetetaanTuloksetWWW(int pisteet, char nikki);


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xBF, 0xFE, 0xE3
};

EthernetClient client;

/* 
                                    USB
                       ----------------------------
   ethernet  D13   17  | PB5(SCK)       PB4(MISO) | 16 D12/MISO ethernet
              3V3      | 3V3            PB3(MOSI) | 15 D11/MOSI ethernet
              AREF 20  | AREF             PB2(SS) | 14 D10      ethernet
   näppis       A0 23  | PC0(ADC0)       PB1(OC1) | 13 D9
   näppis       A1 24  | PC1(ADC1)       PB0(ICP) | 12 D8
   näppis       A2 25  | PC2(ADC2)      PD7(AIN1) | 11 D7
   näppis       A3 26  | PC3(ADC3)      PD6(AIN0) | 10 D6       led
             AD A4 27  | PC4(ADC4)        PD5(T1) | 9 D5        lcd
   Potikka   AD A5 28  | PC5(ADC5)        PD4(T0) | 2 D4        lcd
                A6 19  | ADC6           PD3(INT1) | 1 D3        lcd
             AD A7 22  | ADC7           PD2(IND0) | 32 D2       lcd
   5v                  | 5V              GND      | GND         led
             RESET 29  | PC6(reset)     RESET 29  | PC6(reset)  ethernet irrota uploadin ajaksi  
             GND       |                PD1(TXD)  | 31 D1/TX    lcd Irrotetta uploadin ajaksi
             Vin       | Vin             PD0(RXD) | 30 D0/RX    lcd Irrotetta uploadin ajaksi
                       ----------------------------
                       
*/

// the setup routine runs once when you press reset:
void setup()
{
  //  Nappaimiston pystyjohtimet luetaan C portista biteistä
  //  C0-C3 ( piirilevylla A0-A3)
  //  Näppäimistön vaakajohtimet ohjataan 0:ksi
  //  Linjoilla B0-B3  ( piirilevy D8-D11 )
  DDRC = B11110011;  //  C portti C0-C3 inputiksi
  PORTC = B00001100; //  C portin pull up vastukset päälle alimpiin linjoihin C3-C0

  DDRB = B00001111;

  DDRD = B11111111; // kaikki outputtiin

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  if (Ethernet.begin(mac) == 0)
  {
	  Serial.println("Failed to configure Ethernet using DHCP");
  }
    
	Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  lcd.begin(16, 2);
  lcd.print("    Meteori");
  delay(2000);
  lcd.clear();
} // setup ends



const int RUUDUNPAIVITYS = 50;
const int ALUKSENNOPEUS = 100;
int meteorinNopeus = 130;
bool gameOn = false;
unsigned long pisteet = 0;

////////////////
// Loop start //
////////////////
void loop()
{
  unsigned long pelinAloitus, ajastin1, ajastin2, ajastin3;
  int sensorValue = 0;
  int edellinenSV = 0;
  int cursorY = 0;
  int meteorX = 8;
  int meteorY = 0;
  int shipY = 0;
  int elkut = 0;
  char aakkoset[]= "ABCDEFGHIJKLMNOPQRSTUVXYZÄÖ";
  bool meteoriVaihtanutSuuntaa = false;
  char nikki[] = "";

  // ollaan valikossa kunnes peli alkaa
  while (!gameOn)
  {
    lcd.setCursor(0, 0);
    lcd.print("What do you want?");
    lcd.setCursor(0, 1);
    lcd.print("1: Play 2: Save  ");
    if (luetaanNappain() == 1)
    {

      lcd.clear();
      lcd.print("   LETS PLAY   ");
      delay(1000);
      gameOn = true;
      pisteet = 0;
      ajastin1 = millis();
      ajastin2 = millis();
      ajastin3 = millis();
      pelinAloitus = millis();
      
    }
    else if (luetaanNappain() == 2)
    {

      // luetaan kirjaimia kunnes nikki 3 kirjainta pitka
      while (strlen(nikki) <= 3)
      {
        lcd.clear();
        lcd.print("Nimikirjaimesi:");
        lcd.setCursor(3,1);
        lcd.print(nikki);

        for(int i = 0; i < 25; i++)
        {
          lcd.setCursor(3+strlen(nikki),1);
          lcd.print(aakkoset[i]);
          delay(500);
          if(luetaanNappain()== 1)
          {
            char apu = aakkoset[i];
            char apu2[] = "";
            sprintf(apu2, "%c", apu);
            strcat(nikki,apu2);
            Serial.print("apu: "); Serial.println(apu);      
            Serial.print("nikki: "); Serial.println(nikki);
            break;
          }
        }
      }
        
      // TIEDON LÄHETTÄMINEN NETTIIN
      Serial.println(nikki);
      Serial.println(pisteet);

      char strPisteet[100];
      char data[]="GET http://koti.tamk.fi/~c6tseppa/meteori/add.php?nikki=";
      strcat(data, nikki);
      sprintf(strPisteet, "%d", pisteet);
      strcat(data, "&tulos=");
      strcat(data, strPisteet);
      strcat(data, " HTTP/1.1");
      Serial.println(data);

      Serial.println(Ethernet.localIP());
      if (client.connect("koti.tamk.fi",80)) 
      { // REPLACE WITH YOUR SERVER ADDRESS
        client.println(data);	 
        client.println("Host: koti.tamk.fi"); // SERVER ADDRESS HERE TOO
        client.println("Content-Type: application/x-www-form-urlencoded"); 
        client.println(); // osoittaa kuulemma viestin loppumisen
      
        Serial.println("www loopissa");
	    } 
    else
    {
      lcd.setCursor(0,0);
      lcd.print("Error www connect");
      delay(2000);
      lcd.clear();
    }
    
    delay(1000);
    lcd.clear();
    lcd.print("  Game saved");

    if (client.connected()) 
    { 
      Serial.print("yhteys katkaistu");
      client.stop();	// DISCONNECT FROM THE SERVER
    }
    }
  }

    // Siirrytaan pelaamaan
  while (gameOn)
  {
      Serial.println("----------------------------");
    // Pelin elementtien liikkeiden lasku eriyttamalla
    // saadaan kentan piirto jouhevaksi.
    
    // aluksen liikkeet
    if (millis() - ajastin3 >= ALUKSENNOPEUS)
    {
      Serial.println("alus");
      sensorValue = AD_conversio(7);
      if (sensorValue < edellinenSV - 25)      { shipY = 0; }
      else if (sensorValue > edellinenSV + 25) { shipY = 1;  }

      edellinenSV = sensorValue;
      ajastin3 = millis();
    }
    
    // meteoriitin liikkeiden paivitys ja osumatarkistus
    if (millis() - ajastin1 >= meteorinNopeus)
    {
          Serial.println("meteori");
      if (meteorY == shipY && meteorX == 0)
      { 
        // elamien vahentaminen ja pelin paattaminen
        elkut++;
        Serial.println("Osuma"); 
        if(elkut == 1)
        {
          analogWrite(ledPin,255); 
          Serial.println("elkut 1");
                  meteorX = 8;
          meteorY = random(0, 2);
          meteoriVaihtanutSuuntaa = false;
          meteorinNopeus = random(100, 180);
        }
        else if (elkut == 2) 
        {
          analogWrite(ledPin,30); 
          Serial.println("elkut 2");
          meteorX = 8;
          meteorY = random(0, 2);
          meteoriVaihtanutSuuntaa = false;
          meteorinNopeus = random(100, 180);
        }
        else if (elkut >= 3)
        {
          Serial.println("elkut 3");
          analogWrite(ledPin,0);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("!   BOOOOOOM   !");
          delay(3000);
          lcd.setCursor(0, 0);
          lcd.print("! YOU ARE DEAD !");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Points:");
          lcd.setCursor(0, 1);
          pisteet = ( (millis() -pelinAloitus) / 100);
          lcd.print(pisteet);
          delay(3000);
          gameOn = false;
        }
      }
      else if (meteorX == 0 && meteorY != shipY)
      {
        Serial.println("ohi");
        // uuden meteoriitin asetukset
        meteorX = 8;
        meteorY = random(0, 2);
        Serial.println(meteorY);
        meteoriVaihtanutSuuntaa = false;
        meteorinNopeus = random(100, 180);
        Serial.println(meteorinNopeus);
      } 
      else if (meteoriVaihtanutSuuntaa == false && meteorX <= 5)
      {
        meteorY = random(0, 2);
        meteoriVaihtanutSuuntaa = true;
      } else {  meteorX--;  }

      ajastin1 = millis(); // nollataan ajastin
    }

    // Paivitetaan pelikentta, pisteet. Taajuus nopeampi kuin logiikan laskulla.
    // toivottavasti pisteet rullaavat oikein
    if (millis() - ajastin2 >= RUUDUNPAIVITYS)
    {
      Serial.println("ruudunpaivitys");
      lcd.clear();

      lcd.setCursor(9, 0);
      lcd.print("Pisteet:");

      lcd.setCursor(9, 1);
      lcd.print((millis() -pelinAloitus) / 100);

      lcd.setCursor(0, shipY);
      lcd.print("D");
      lcd.setCursor(meteorX, meteorY);
      lcd.print("o");
      ajastin2 = millis();

    }
  }
} // LOOP ENDS

///////////////////////////////////////////////////////////
// Nappiamistolta luku palauttaa -1, nappia ei painettu  //
///////////////////////////////////////////////////////////
char luetaanNappain()
{

  // correction table to return right key value
  int korjaustaulukko[] = {
      5, 4,
      2, 1,

  };

  byte x_mask, y_mask, my_var;
  int key;

  do
  {
    key = -1; // changes only if key pressed

    y_mask = B11111110;

    for (int y = 0; y < 2; y++)
    {
      PORTC = y_mask & B00001111;
      x_mask = B11111011;

      for (int x = 0; x < 2; x++)
      {
        my_var = PINC;
        my_var = my_var & B00001100;

        if (my_var == (x_mask & B00001100)) // tulostaa ekan rivin koska nollaa liikutellaan
        {
          return korjaustaulukko[x + y * 2];
        }

        x_mask <<= 1;
        x_mask++;
      }

      y_mask <<= 1;
      y_mask++;
    }

    // jos mitään nappia ei silmukoissa tunnistettu palautetaan -1
    if (key == -1)
    {
      return -1;
    }
  } while (key == -1);

} // nappi osuus loppuu ohjelma hyppää loopin alkuun

/////////////////////////
// Potikan AD muunnos  //
/////////////////////////
int AD_conversio(byte ch)
{
  // kahdella ekalla bitilla valitaan referenssi jannite
  // seuraavilla mitka analogiset portit liittyvat ADC rekisteriin(= Nanon portit)
  // ADMUX=B01000101; // ch4 ja 5v ref
  ADMUX = B01000000 | ch; // referenssi jannite 5v ja input kanava ch

  // Kaannoksen hallintaan 8bit rekisteri
  // Ad aktivointi|kaannoksen aloitus|auto trigger enable|interrupt flag|interrupt enable|3*kellontaajuuden saato
  // asetetaan ad muunnos paalle ja aloitetaan yksinkertainen kaannos
  ADCSRA = B11000111;

  while ((ADCSRA & B01000000) != 0)
    ;         // odotellaan kunnes konversio paattynyt
  return ADC; // palautetaan AD muunnettu arvo.
}

/////////////////////////////////////
//  Tulosten nettiin lahettaminen  //
/////////////////////////////////////
void lahetetaanTuloksetWWW(int pisteet, char nikki) 
{
	// Muotoillaan sql tietokannalle tarkoittettu parametrit ja niiden arvot sisaltava viesti data muuttujaan
	// koska c:ssa ei stringeja kaytossa jouduin hieman kikkailemaan funktioilla, etta sain viestin 
	// parsittua kasasaan. Lahtokohtaisesti oletin, etta olisi toiminut sana + sana = sanasana logiikalla
	// mutta niin ei ollutkaan.
	char strPisteet[100];
	char data[200]="GET http://koti.tamk.fi/~c6tseppa/meteori/add.php?nikki=";
	sprintf(strPisteet, "%d", pisteet);
	strcat(data, nikki);
	strcat(data, "&tulokset=");
	strcat(data, strPisteet);
	strcat(data, "HTTP/1.1");
	printf("Data: %s\n",data);

	// muodosttetaan yhteys tamkkin serverille
	if (client.connect("koti.tamk.fi",80))
	{
		// ja lahetetaan http/1.1 GET viesti, jolla tiedot lahetetaan serverille tietokantaan vietavaksi
		client.println(data);	// data sis. rakennettu GET viesti serverille 
		client.println("Host: koti.tamk.fi");
		client.println("Content-Type: application/x-www-form-urlencoded"); 
		client.println(); // tyhja rivi vaaditaan osoittamaan viestin loppuminen
	
		// varmistus sarja portitin, etta loopissa on kayty
		Serial.println("loopissa");
	} 
	else
	{
		// tulostaan naytolle virheviesti, jos tulosten lahettaminen epaonnistui.
		lcd.setCursor(0,0);
		lcd.print("Error www connect");
		delay(2000);
		lcd.clear();
	}

	if (client.connected()) 
	{ 
		Serial.print("yhteys katkaistu");
		client.stop();	// katkaistaan yhteys serveriin
	}
}