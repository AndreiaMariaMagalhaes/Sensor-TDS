
#define TdsSensorPin A1
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30          // define o número de posições a guardar
int analogBuffer[SCOUNT];    // declara um conjunto de SCOUNT posições 
int analogBufferTemp[SCOUNT]; // declara um conjunto temporário para ordenação dos valores nas suas posições
int analogBufferIndex = 0,copyIndex = 0; // declara variáveis necessárias para cálculo e atribui zero a essa variável
float averageVoltage = 0,tdsValue = 0,temperature = 25; // declara variáveis necessárias para cálculo e atribui zero a essa variável

void setup() // define a entrada e saída de dados
{
    Serial.begin(115200); // defição da taxa de transmissão de valores
    pinMode(TdsSensorPin,INPUT); // define um pino como sendo Analógico 1 (A1)
}

void loop()// define o ciclo para medição constante de valores
{
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U) //a cada 40 milisegundos efetua a leitura do valor analógico do sensor
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //efetua a leitura do valor analógico e adiciona ao conjunto de SCOUNT leituras
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      // efetua a leitura do valor analógico mais estável pela mediana do algoritmo e converte em valor em volt
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; 
      //fórmula de compensação de temperatura: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient=1.0+0.02*(temperature-25.0);   
      //compensação de temperatura
      float compensationVolatge=averageVoltage/compensationCoefficient;
      //converte o valor em volt para o valor em TDS
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;

      Serial.println("******************************************************************"); // Área de impressão de informação
      Serial.print("Voltagem: ");
      Serial.print(averageVoltage,2);
      Serial.println(" V");
      
      Serial.print("Valor do sensor: ");
      Serial.print(tdsValue,0);
      Serial.print(" ppm  = ");
      Serial.print(tdsValue*0.001,3);
      Serial.println(" g/L");
      
      Serial.print("Descrição: ");
      if(tdsValue>0 && tdsValue<=50){
        Serial.println(" - Água desionizada, destilada, microfiltração");
      }
      if(tdsValue>50 && tdsValue<170){
        Serial.println(" - Água de nascente de montanha ou aquífero");
      }
      if(tdsValue >170 && tdsValue<420){
        Serial.println(" - Água de torneira (normal)");
      }
      if(tdsValue>500){
        Serial.println(" - Água contaminada");
      }
      if(tdsValue>420 && tdsValue<500){
        Serial.println(" - Água de nascente ou da torneira com elevado valor de sólidos totais dissolvidos");
      }
      Serial.print("Classificação: ");
      if(tdsValue >0 && tdsValue<=70){
        Serial.println(" - Água muito mole");
      }
      if(tdsValue >70 && tdsValue<=135){
        Serial.println(" - Água mole (branda)");
      }
      if(tdsValue >135 && tdsValue<=200){
        Serial.println(" - Água de dureza média");
      }
      if(tdsValue >200 && tdsValue<=350){
        Serial.println(" - Água dura");
      }
      if(tdsValue >350){
        Serial.println(" - Água muito dura");
      }
   }
}
// algoritmo de ordenação
int getMedianNum(int bArray[], int iFilterLen)
{
      int bTab[iFilterLen]; 
      for (byte i = 0; i < iFilterLen; i++)
      bTab[i] = bArray[i];                 
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
       }
      }
      if ((iFilterLen & 1) > 0)
        bTemp = bTab[(iFilterLen - 1) / 2];
      else
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
