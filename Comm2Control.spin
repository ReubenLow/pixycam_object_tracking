{Object_Title_and_Purpose}
CON

  _clkmode = xtal1 + pll16x                                               'Standard clock mode * crystal frequency = 80 MHz
  _xinfreq = 5_000_000
  _ConClkFreq = ((_clkmode - xtal1) >> 6) * _xinfreq
  _Ms_001 = _ConClkFreq / 1_000


  cmdStart     = $50
  cmdStop      = $0B
  cmdForward   = $01
  cmdReverse   = $02
  cmdLeft      = $03
  cmdRight     = $04
  cmdSideLeft  = $05
  cmdSideRight = $06
  cmdTopLeft   = $07
  cmdTopRight  = $08
  cmdBotLeft   = $09
  cmdBotRight  = $0A
  'STM baud rate
  STMBaud = 115200

  'check values
  ok = $0F   'if no obstacle
  ntok =$0E  'if obstacle

OBJ
  Def           : "Definitions.spin"
  Comm          : "FullDuplexSerialExt.spin"
VAR
  long MainHubMS
  long  cog, cogStack[64]

PUB Stop 'stop core
  if(cog)
    cogstop(~cog - 1)
  return

PUB Start(MS, commMem, Time, State, Value, Check, Checksum)       'start core form mylitekit
  mainHubMS := MS
  Stop
  cog := cognew(commCore(commMem, Time, State, Value, Check, Checksum), @cogStack) + 1
  return cog

PRI Pause(ms) | t
  t := cnt - 1088                                               ' sync with system counter
  repeat (ms #> 0)                                              ' delay must be > 0
    waitcnt(t += mainHubMS)
  return

PUB commCore(commMem, Time, State, Value, Check, Checksum) | i,j, k, l, direction, Speed, startbit  'receive and transmitt via STM32

  Comm.Start(12, 13, 0, STMBaud)  'stm32 pinout rx(PA10,BLUE), tx(PA9, GREEN)         start(rxpin, txpin, mode, baudrate)

  repeat
    k := long[State]    'ini state into k
    Comm.tx(k)          'trasmit k, for stm 32 to check
    startbit := Comm.Rx 'store 1st byte for check of start of transmission
    if startbit == $14
      direction := Comm.Rx 'store 2nd byte of recived value into direction
      long[Time] := $64
      long[Value] := Comm.Rx 'store 3rd byte of recived value into time
      long[Check] := Comm.Rx       'store 4th byte of recived value into check
      long[Checksum] := (direction + startbit + long[Value]) 'make checksum value
          if long[Checksum] == long[Check]    'validate checksum
            j := direction        'switch case
            case j
              cmdStart:
                long[commMem][0] := 0
              cmdForward:
                long[commMem][0] := 1
              cmdReverse:
                long[commMem][0] := 2
              cmdLeft:
                long[commMem][0] := 3
              cmdRight:
                long[commMem][0] := 4
              cmdSideLeft:
                long[commMem][0] := 5
              cmdSideRight:
                long[commMem][0] := 6
              cmdTopLeft:
                long[commMem][0] := 7
              cmdTopRight:
                long[commMem][0] := 8
              cmdBotLeft:
                long[commMem][0] := 9
              cmdBotRight:
                long[commMem][0] := 10
             cmdStop:
                long[commMem][0] := 11

    Pause(1)