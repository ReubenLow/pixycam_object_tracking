{Object_Title_and_Purpose}

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
  _ConClkFreq = ((_clkmode - xtal1) >> 6) * _xinfreq
  _Ms_001   = _ConClkFreq / 1_000

  ToF_limit = 00
  Front_limit = 330   '0
  Back_limit = 330      '1
  Left_limit = 330       '02
  Right_limit = 330    '03
  Ultra_limit = 300
  'check values
  ok = $0F
  ntok = $0E

VAR
  ' Motors
  long Cmd, Pulse, Speed

  ' Sensors
  long ToF[2], Ultra[4]

  ' Wireless Communication
  long  Comm[16], Time, State, Value, Check, Checksum

OBJ
  Def   : "Definitions.spin"
  pst   : "Parallax Serial Terminal.spin"
  Mot   : "MecanumControl.spin"
  Sen   : "SensorMuxControl.spin"
  Com   : "Comm2Control.spin"

PUB Main | Block                      ' For motorcontrol, Cmd 1: Forward, Cmd 2: Reverse, Cmd 3: Turn Left, Cmd 4: Turn Right, Cmd 5: Stop


  pst.Start(115200) 'stm 32 baud as well
  Pause(1000)

  Mot.Start(_Ms_001, @Cmd, @Pulse, @Speed)

  Sen.Start(_Ms_001, @Tof, @Ultra)

  Com.Start(_Ms_001, @Comm, @Time, @State, @Value, @Check, @Checksum)

  pst.Str(String("Start"))  'for debugging
  pst.Chars(pst#NL, 2)


  repeat
    {pst.Str(String("ToF 1:"))                     'to check for debugging
    pst.Dec(ToF[0])
    pst.Chars(pst#NL, 2)
    pst.Str(String("ToF 2:"))
    pst.Dec(ToF[1])
    pst.Chars(pst#NL, 2)
    pst.Str(String("Ultra 1:"))       'front
    pst.Dec(Ultra[0])
    pst.Chars(pst#NL, 2)
    pst.Str(String("Ultra 2:"))       'back
    pst.Dec(Ultra[1])
    pst.Chars(pst#NL, 2)
    pst.Str(String("Ultra 3:"))       'left
    pst.Dec(Ultra[2])
    pst.Chars(pst#NL, 2)
    pst.Str(String("Ultra 4:"))       'right
    pst.Dec(Ultra[3])
    pst.Chars(pst#NL, 2) }

    'State := ok
     long[@State] := ok       'always initalize state to ok

    Pulse := long[@Time]        'initiallise time from comm2control to pulse to be used in mechanum control
    Speed := long[@Value]
    'pst.Dec(long[@Check])
    'pst.Chars(pst#NL, 2)
    'pst.Dec(long[@Checksum])
    'pst.Chars(pst#NL, 2)
    case long[@Comm][0]

      '0: 'start  if required

      1:
        if(long[@Ultra][0] < Ultra_limit) 'OR (long[@ToF][0] > ToF_limit)             ' If obstacle detected, stop         'comment out tof if required
            long[@State] := ntok 'initaise state to ntok if obstacle detected, follow suit for all obstacle detection
            Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
        'else
        if(long[@Ultra][0] > Ultra_limit) 'AND (long[@ToF][0] < ToF_limit)
                              long[@State] := ok
                              Cmd := 1
                                 pst.Str(String("Forward"))
                                   pst.Chars(pst#NL, 2)

      2:
          if(long[@Ultra][1] < Ultra_limit) 'OR (long[@ToF][1] > ToF_limit)             ' If obstacle detected, stop
             long[@State] := ntok
             Cmd := 11
                  pst.Str(String("Stop"))
                  pst.Chars(pst#NL, 2)
       'else
        if(long[@Ultra][1] > Ultra_limit)' AND (long[@ToF][1] < ToF_limit)
                              long[@State] := ok
                              Cmd := 2
                                 pst.Str(String("Backward"))
                                   pst.Chars(pst#NL, 2)

      3:
        if(long[@Ultra][2] < Ultra_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
       'else
        if(long[@Ultra][2] > Ultra_limit)
                                long[@State] := ok
                                Cmd := 3
                                   pst.Str(String("Turn Left"))
                                     pst.Chars(pst#NL, 2)
      4:
        if(long[@Ultra][3] < Ultra_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
        'else
        if(long[@Ultra][3] > Ultra_limit)
                                long[@State] := ok
                                Cmd := 4
                                   pst.Str(String("Turn Right"))
                                     pst.Chars(pst#NL, 2)
      5:
        if(long[@Ultra][2] < Ultra_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
        'else
        if(long[@Ultra][2] > Ultra_limit)
                                long[@State] := ok
                                Cmd := 5
                                    pst.Str(String("Side Left"))
                                      pst.Chars(pst#NL, 2)
      6:
        if(long[@Ultra][3] < Ultra_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
       'else
        if(long[@Ultra][3] > Ultra_limit)
             long[@State] := ok
             Cmd := 6
              pst.Str(String("Side Right"))
                pst.Chars(pst#NL, 2)
      7:
        if(long[@Ultra][2] < Ultra_limit) OR (long[@Ultra][0] < Ultra_limit)' OR (long[@ToF][0] > ToF_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
               pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)
        'else
        if(long[@Ultra][2] > Ultra_limit) AND (long[@Ultra][0] > Ultra_limit) ' AND (long[@ToF][0] > ToF_limit)
                                long[@State] := ok
                                Cmd := 7
                                   pst.Str(String("Top Left"))
                                     pst.Chars(pst#NL, 2)
      8:
        if(long[@Ultra][3] < Ultra_limit) OR (long[@Ultra][0] < Ultra_limit)' OR (long[@ToF][0] > ToF_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
                 pst.Str(String("Stop"))
                 pst.Chars(pst#NL, 2)
        'else
        if(long[@Ultra][3] > Ultra_limit) AND (long[@Ultra][0] > Ultra_limit) ' AND (long[@ToF][0] > ToF_limit)
                                long[@State] := ok
                                Cmd := 8
                                   pst.Str(String("Top Right"))
                                   pst.Chars(pst#NL, 2)
      9:
         if(long[@Ultra][2] < Ultra_limit) OR (long[@Ultra][1] < Ultra_limit)' OR (long[@ToF][1] > ToF_limit)             ' If obstacle detected, stop
           long[@State] := ntok
           Cmd := 11
                 pst.Str(String("Stop"))
                 pst.Chars(pst#NL, 2)
         'else
        if(long[@Ultra][2] > Ultra_limit) AND (long[@Ultra][1] > Ultra_limit) ' AND (long[@ToF][1] > ToF_limit)
                                long[@State] := ok
                                Cmd := 9
                                   pst.Str(String("Bottom left"))
                                   pst.Chars(pst#NL, 2)
      10:
        if(long[@Ultra][3] < Ultra_limit) OR (long[@Ultra][1] < Ultra_limit)' OR (long[@ToF][1] > ToF_limit)             ' If obstacle detected, stop
          long[@State] := ntok
          Cmd := 11
                 pst.Str(String("Stop"))
                 pst.Chars(pst#NL, 2)
       'else
        if(long[@Ultra][3] > Ultra_limit) AND (long[@Ultra][1] > Ultra_limit) ' AND (long[@ToF][1] > ToF_limit)
                                long[@State] := ok
                                Cmd := 10
                                   pst.Str(String("Bottom Right"))
                                   pst.Chars(pst#NL, 2)
      11:
        Cmd := 11
         pst.Str(String("Stop"))
               pst.Chars(pst#NL, 2)

   Pause(1)

PRI Pause(ms) | t
  t := cnt - 1088                                               ' sync with system counter
  repeat (ms #> 0)                                              ' delay must be > 0
    waitcnt(t += _MS_001)
  return