{Object_Title_and_Purpose}{
Author: Reuben
Ultrasonic and Time of Flight

}
CON
        _clkmode = xtal1 + pll16x                                               'Standard clock mode * crystal frequency = 80 MHz
        _xinfreq = 5_000_000
        _ConClkFreq = ((_clkmode - xtal1) >> 6) * _xinfreq
        _Ms_001 = _ConClkFreq / 1_000

        ' OLD PINOUTS
        {'Time of Flight Pins
        ToF1SCL = 4
        ToF1SDA = 5
        ToF1RST = 6

        ToF2SCL = 11
        ToF2SDA = 12
        ToF2RST = 13

        'Ultrasonic Pins
        Ultra1SCL = 8
        Ultra1SDA = 9
        Ultra2SCL = 20
        Ultra2SDA = 21   }

        ' TCA9548A Pins
        TCARST = 10

        ' Time of Fight Pins
        ToF1RST = 6
        ToF2RST = 7

        ' Ultrasonic Pins


        ' TCA9548A I2C MUX Address
        MuxAddr = $70                                   ' Default I2c address

        ' ToF address
        ToFAddr = $29

        ' Ultra address
        UltraAddr = $57

        ' TCA9548A I2C MUX Reset
        EndState = 0                                    ' Reset pin of MUX is pulled high by default so to reset, pull to low
VAR
  long  mainHubMS, cog, cogStack[128]

  ' Debugging
  long  tofMem[2], ultraMem[4]

OBJ
  'Def           : "Definitions.spin"
  'tof[2]        : "ToF.spin"
  'ultra         : "Ultrasonic_v3.spin"
  'pst           : "Parallax Serial Terminal"
  TCA           : "TCA9548Av2.spin"

PUB Stop
{{ Check & Stop/Unload Core if loaded }}
  if cog
    cogstop(~cog - 1)
  return

PUB Start(MS, mainToFAdd, mainUltraAdd)
{{ Load Sensor Core }}
  mainHubMS := MS
  Stop
  cog := cognew(SensorCore(mainToFAdd, mainUltraAdd), @cogStack) + 1
  return cog

PUB SensorCore(ToFSenAdd, UltraSenAdd) | ToF_Range1, ToF_Range2, UltraRange1, UltraRange2, UltraRange3, UltraRange4

  'pst.start(115200)
  'pst.Str(String("Test"))
  WAITCNT((2*(clkfreq/1000)) + cnt)

  TCA.PInit2                                            ' Initialise I2C bus lines _PCA9548A_SCL and _PCA9548A_SDA

  TCA.PSelect(0, 0)                                     ' Select channel 0 - Front ToF
  TCA.initVL6180X(ToF1RST)                              ' Initialise reset pin for ToF 1
  TCA.ChipReset(1, ToF1RST)                             ' Perform hardware reset on TCA9548A, restart the chip or power on reset
  'Pause(1000)
  Pause(500)
  TCA.FreshReset(ToFAddr)                               ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.
  TCA.MandatoryLoad(ToFAddr)                            ' Configure bytes to addresses within the MUX
  TCA.RecommendedLoad(ToFAddr)                          ' Configure bytes to addresses within the MUX
  TCA.FreshReset(ToFAddr)                               ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.

  Pause(500)

  TCA.PSelect(1, 0)                                     ' Select channel 1 - Back ToF
  TCA.initVL6180X(ToF2RST)                              ' Initialise reset pin for ToF 2
  TCA.ChipReset(1, ToF2RST)                             ' Perform hardware reset on TCA9548A, restart the chip or power on reset
  'Pause(1000)
  Pause(500)
  TCA.FreshReset(ToFAddr)                               ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.
  TCA.MandatoryLoad(ToFAddr)                            ' Configure bytes to addresses within the MUX
  TCA.RecommendedLoad(ToFAddr)                          ' Configure bytes to addresses within the MUX
  TCA.FreshReset(ToFAddr)                               ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.


  repeat
    'ToF_Range1 := TCA.GetSingleRange(...)
    'pst.Str(String("TOF 1 -> Getting range"))
    'pst.Chars(pst#NL, 2)
    TCA.PSelect(0, 0)                                                           ' Select channel 0 - Front ToF
    long[ToFSenAdd][0] := TCA.GetSingleRange(ToFAddr)
    Pause(1)
    'ToF_Range1 := TCA.GetSingleRange(MuxAddr)
    'pst.Dec(ToF_Range1)

    TCA.PSelect(1, 0)                                                           ' Select channel 1 - Back ToF
    long[ToFSenAdd][1] := TCA.GetSingleRange(ToFAddr)
    Pause(1)

    TCA.PSelect(2, 0)
    TCA.PWriteByte(2, UltraAddr, $01)                                           ' Trigger sensor
    Pause(30)
    long[UltraSenAdd][0] := TCA.readHCSR04(2, UltraAddr)*100/254                ' 100/254, compensation value to get accurate readings
    Pause(1)
    TCA.resetHCSR04(2, UltraAddr)

    TCA.PSelect(3, 0)
    TCA.PWriteByte(3, UltraAddr, $01)                                           ' Trigger sensor
    Pause(30)
    long[UltraSenAdd][2] := TCA.readHCSR04(3, UltraAddr)*100/254                ' 100/254, compensation value to get accurate readings
    Pause(1)
    TCA.resetHCSR04(3, UltraAddr)

    TCA.PSelect(4, 0)
    TCA.PWriteByte(4, UltraAddr, $01)                                           ' Trigger sensor
    Pause(30)
    long[UltraSenAdd][3] := TCA.readHCSR04(4, UltraAddr)*100/254                ' 100/254, compensation value to get accurate readings
    Pause(1)
    TCA.resetHCSR04(4, UltraAddr)

    TCA.PSelect(5, 0)
    TCA.PWriteByte(5, UltraAddr, $01)                                           ' Trigger sensor
    Pause(30)
    long[UltraSenAdd][4] := TCA.readHCSR04(5, UltraAddr)*100/254                ' 100/254, compensation value to get accurate readings
    Pause(1)
    TCA.resetHCSR04(5, UltraAddr)

   Pause(5)

PRI tofInit(channel) | i
{{ Init ToF Sensors via TCP9548A }}

  case channel
    0:
      TCA.initVL6180X(ToF1RST)                       ' Initialise reset pin for ToF 1
      TCA.ChipReset(1, ToF1RST)                      ' Perform hardware reset on TCA9548A, restart the chip or power on reset
      'Pause(1000)
      Pause(500)
      TCA.FreshReset(ToFAddr)                        ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.
      TCA.MandatoryLoad(ToFAddr)                     ' Configure bytes to addresses within the MUX
      TCA.RecommendedLoad(ToFAddr)                   ' Configure bytes to addresses within the MUX
      TCA.FreshReset(ToFAddr)                        ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.

    1:
      TCA.initVL6180X(ToF2RST)                       ' Initialise reset pin for ToF 1
      TCA.ChipReset(1, ToF2RST)                      ' Perform hardware reset on TCA9548A, restart the chip or power on reset
      'Pause(1000)
      Pause(500)
      TCA.FreshReset(ToFAddr)                        ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.
      TCA.MandatoryLoad(ToFAddr)                     ' Configure bytes to addresses within the MUX
      TCA.RecommendedLoad(ToFAddr)                   ' Configure bytes to addresses within the MUX
      TCA.FreshReset(ToFAddr)                        ' Perform software reset on the MUX , bring it to active state, eliminate potential issues etc.
  return

PRI Pause(ms) | t
  t:=cnt - 1088
  repeat (ms#>0)
    waitcnt(t += mainHubMS)
  return