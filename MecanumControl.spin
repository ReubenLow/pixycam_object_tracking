{Object_Title_and_Purpose}{
Author: Reuben
MecanumControl.spin contains 11 mecanum movement functions.

}
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
  _ConClkFreq = ((_clkmode - xtal1) >> 6) * _xinfreq
  _Ms_001   = _ConClkFreq / 1_000


  ' RoboClaw 1
        R1S1 = 3                      'Left front wheel, Chn 1
        R1S2 = 2                      'Right front wheel, Chn 2
  ' RoboClaw 2
        R2S1 = 5                      'Left back wheel, Chn 1
        R2S2 = 4                      'Right back wheel, Chn 2

  ' Simple Serial
        SSBaud = 57_600

  ' Motor Speed
        C1_For = 120
        C2_For = 248
        C1_Rev = 8
        C2_Rev = 135
        C1_Stop = 64
        C2_Stop = 192

   'Standard serial command syntax reference
   '     0   : Shuts Down Channel 1 & 2
   '     1   : Channel 1 - Full Reverse
   '     64  : Channel 1 - Stop
   '     127 : Channel 1 - Full Forward
   '     128 : Channel 2 - Full Reverse
   '     192 : Channel 2 - Stop
   '     255 : Channel 2 - Full Forward

VAR
  long  cog, cogStack[64]

OBJ
  MD1   : "FullDuplexSerialExt.spin"
  MD2   : "FullDuplexSerialExt.spin"
  'DBG   : "FullDuplexSerialExt.spin"      ' for serial monitoring (debug)

{PUB Start(MS, Cmd, Duration)                                   ' Start a new core

  Stop
  cog := cognew(motorCore(Cmd, Duration), @cogStack) + 1          ' create new cog
  return cog


PUB Stop
{{ Stop & Release Core }}
  if cog
    cogstop(cog~ - 1)
  return                                }
PRI Pause(ms) | t
  t := cnt - 1088                                               ' sync with system counter
    repeat (ms #> 0)                                              ' delay must be > 0
      waitcnt(t += _MS_001)
    return

PUB motorCore(Cmd, Duration) | i
'PUB motorCore(Duration)
  MD1.start(R1S2, R1S1, 0, SSBaud)
  MD2.start(R2S2, R2S1, 0, SSBaud)
  'DBG.Start(31, 30, 0, 57600)

  Forward(1000)
  'Pause(1000)
  'StopMotors
  'Reverse(2000)
  'TurnLeft(500)
  'TurnRight(500)
  'SideLeft(100)
  'SideRight(500)
  'DiaTopLeft(500)
  'DiaTopRight(500)
  'DiaBotLeft(500)
  'DiaBotRight(500)

  'Pause(500)
  'StopMotors
 {
   repeat
    case long[Cmd]
      1:
        Forward(long[Duration])
      2:
        Reverse(long[Duration])
      3:
        TurnLeft(long[Duration])
      4:
        TurnRight(long[Duration])
      5:
        SideLeft(long[Duration])
      6:
        SideRight(long[Duration])
      7:
        DiaTopLeft(long[Duration])
      8:
        DiaTopRight(long[Duration])
      9:
        DiaBotLeft(long[Duration])
      10:
        DiaBotRight(long[Duration])
      11:
        StopMotors
      Pause(5)                                    }
  return

PUB Forward(Duration) | i, StartTime
  if Duration < 1                                    ' Check for invalid duration
    ' Invalid duration
    return
                                                     ' Set StartTime to current value of the system clock represented by cnt, cnt increment by a count per clock cycle.
  StartTime := cnt
  repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_For)
    Pause(Duration)                                  ' Loop until difference between current clock time and StartTime is greater than duration specified
  until((cnt - StartTime) => Duration)               ' Stop motors if difference is greater than duration
  StopMotors
   return

{PUB Forward1(Duration)    | i, StartTime          ' For testing and debugging
  if Duration < 1                                  ' Check for invalid duration
    ' Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_For)
    Pause(Duration)
  until((cnt - StartTime) => Duration)
  StopMotors
     return
    'Pause(Duration)                                ' If CNT does not work again somehow, use this
    'return

      repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_For)
    Pause(Duration)
     if((cnt - StartTime) => Duration)
       StopMotors
       return
    'Pause(Duration)                                ' If CNT does not work again somehow, use this
    'return

  {repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_For)
    Pause(Duration)
      return          }

  return         }
PUB Reverse(Duration)    | i, StartTime

  if Duration < 1                                   ' Check for invalid duration
    ' Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Rev)
    MD1.Tx(C2_Rev)
    MD2.Tx(C1_Rev)
    MD2.Tx(C2_Rev)
    Pause(Duration)
  until((cnt - StartTime) => Duration)
  StopMotors
    'Pause(Duration)                                ' If CNT does not work again somehow, use this
  return
PUB TurnRight(Duration)   | i, StartTime     ' Forward channel 2, reverse channel 1
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Rev)
    MD1.Tx(C2_For)
    MD2.Tx(C1_Rev)
    MD2.Tx(C2_For)
  until((cnt - StartTime) => Duration)
  StopMotors
  return

PUB TurnLeft(Duration)   | i, StartTime     ' Forward channel 2, reverse channel 1
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_Rev)
    MD2.Tx(C1_For)
    MD2.Tx(C2_Rev)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB SideRight(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Rev)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_Rev)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB SideLeft(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_Rev)
    MD2.Tx(C1_Rev)
    MD2.Tx(C2_For)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB DiaTopRight(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Stop)
    MD1.Tx(C2_For)
    MD2.Tx(C1_For)
    MD2.Tx(C2_Stop)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB DiaTopLeft(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_For)
    MD1.Tx(C2_Stop)
    MD2.Tx(C1_Stop)
    MD2.Tx(C2_For)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB DiaBotRight(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Rev)
    MD1.Tx(C2_Stop)
    MD2.Tx(C1_Stop)
    MD2.Tx(C2_Rev)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB DiaBotLeft(Duration)   | i, StartTime
  if Duration < 1
    'Invalid duration
    return

  StartTime := cnt
  repeat
    MD1.Tx(C1_Stop)
    MD1.Tx(C2_Rev)
    MD2.Tx(C1_Rev)
    MD2.Tx(C2_Stop)
  until((cnt - StartTime) => Duration)
  StopMotors
  return
PUB StopMotors
  MD1.Tx(C1_Stop)
  MD1.Tx(C2_Stop)
  MD2.Tx(C1_Stop)
  MD2.Tx(C2_Stop)
  return

'DAT ' code to allow user to input speed, might implement in the future.

 {PUB Forward(Duration, Speed)    | i, StartTime

  if Speed < 65 or Speed > 127                     ' Check for invalid speed
    ' Invalid byte
    return
  if Duration < 1                                  ' Check for invalid duration
    ' Invalid duration
    return

  i := Speed + 128                                 ' Increment input speed to get forward byte for channel 2

  if i > 255                                       ' Double check for invalid speed and correct the speed value if needed
    i := 255

  StartTime := CNT
  repeat
    MD1.Tx(Speed)
    MD1.Tx(i)
    MD2.Tx(Speed)
    MD2.Tx(i)
    if (CNT - StartTime) => Duration
      return
    'Pause(Duration)                                ' If CNT does not work somehow, use this
  return

PUB Reverse(Duration, Speed)    | i, StartTime

  if Speed < 1 or Speed > 63                        ' Check for invalid speed
    ' Invalid byte
    return
  if Duration < 1                                   ' Check for invalid duration
    ' Invalid duration
    return

  i := Speed + 128                                  ' Increment input speed to get reverse byte for channel 2

  if i > 128                                        ' Double check for invalid speed and correct the speed value if needed
    i := 128

  StartTime := CNT
  repeat
    MD1.Tx(Speed)
    MD1.Tx(i)
    MD2.Tx(Speed)
    MD2.Tx(i)
    if (CNT - StartTime) => Duration
      return
    'Pause(Duration)                                ' If CNT does not work somehow, use this
  return}

{PUB TurnRight(Duration, Speed)   | i, StartTime     ' Forward channel 2, reverse channel 1

  if Speed < 65 or Speed > 127                     ' Check for invalid speed
    ' Invalid byte
    return
  if Duration < 1
    'Invalid duration
    return

  i := (127 - Speed) + 128                          ' Reverse byte for channel 1
  StartTime := CNT
  if Speed == 127
    i := 128
  repeat
    MD1.Tx(i)
    MD1.Tx(Speed)
    MD2.Tx(i)
    MD2.Tx(Speed)
    if (CNT - StartTime) => Duration
      return
  return         }