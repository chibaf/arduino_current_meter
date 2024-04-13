// 端子定義
const int arefPin = 0;
const int voltagePin = 1;
const int currentPin = 2;

// 商用電源周波数
#define POWER_FREQ        (60)

// １サイクルあたりのサンプル数
#define NUMBER_OF_SAMPLES (25)

// サンプリング間隔(マイクロ秒)
#define SAMPLING_PERIOD   (1000000/(POWER_FREQ * NUMBER_OF_SAMPLES))

// デバッグ用
#define DEBUG 1

// 実効電圧、実効電流、有効電力
float Vrms;
float Irms;
float Watt;

// サンプリング用バッファ
int VASamples[NUMBER_OF_SAMPLES*4];

void calcWatt(void)
{
#define kVT    (86.9817579)    // 実測にもとづく係数
#define kCT    (100.0 * 0.99 / 800.0) // R * 係数 / 巻き数

  unsigned long t1,t2;
  int i,r,v1,a1,a2,v2;

  t1 = micros();

  // １サイクル分のAD値をサンプリング
  for(i=0; i<NUMBER_OF_SAMPLES; i++){

    r = analogRead(arefPin);
    v1 = analogRead(voltagePin);
    a1 = analogRead(currentPin);
    a2 = analogRead(currentPin);
    v2 = analogRead(voltagePin);

    VASamples[(i*4)+0] = v1 - r;
    VASamples[(i*4)+1] = a1 - r;
    VASamples[(i*4)+2] = a2 - r;
    VASamples[(i*4)+3] = v2 - r;

    do {
      t2 = micros();
    } 
    while((t2 - t1) < SAMPLING_PERIOD);
    t1 += SAMPLING_PERIOD;
  }

  // １サイクル分の電圧と電流、電力を加算
  Vrms = 0;
  Irms = 0;
  Watt = 0;

  for(i=0; i<NUMBER_OF_SAMPLES; i++){
    v1 = VASamples[(i*4)+0];
    a1 = VASamples[(i*4)+1];
    a2 = VASamples[(i*4)+2];
    v2 = VASamples[(i*4)+3];

    float vv = ((((v1+v2)/2) * 5.0) / 1024) * kVT;
    float aa = ((((a1+a2)/2) * 5.0) / 1024) / kCT;

    Vrms += vv * vv;
    Irms += aa * aa;
    Watt += vv * aa;
  }
