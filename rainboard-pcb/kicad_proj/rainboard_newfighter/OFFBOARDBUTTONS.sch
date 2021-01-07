EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr D 34000 22000
encoding utf-8
Sheet 65 65
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 4700 1800 0    150  ~ 30
TODO SELECT CONNECTOR\n
Text GLabel 1700 2500 0    50   Input ~ 0
BTNX-1
Text GLabel 1700 3150 0    50   Input ~ 0
BTNX-2
Text GLabel 1700 3800 0    50   Input ~ 0
BTNX-3
Text GLabel 1700 4450 0    50   Input ~ 0
BTNX-4
Text GLabel 1700 5100 0    50   Input ~ 0
BTNX-5
Text GLabel 1700 5750 0    50   Input ~ 0
BTNX-6
Text GLabel 3100 2500 2    50   Input ~ 0
BTNX-7
Text GLabel 3100 3150 2    50   Input ~ 0
BTNX-8
Text GLabel 3100 3800 2    50   Input ~ 0
BTNX-9
Text GLabel 3100 4450 2    50   Input ~ 0
BTNX-10
Text GLabel 3100 5100 2    50   Input ~ 0
BTNX-11
Text GLabel 3100 5750 2    50   Input ~ 0
BTNX-12
$Comp
L lib_symbol:Conn_01x02_Female J10
U 1 1 6574D0B9
P 2150 2500
F 0 "J10" H 2050 2600 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 2385 50  0001 L CNN
F 2 "" H 2150 2500 50  0001 C CNN
F 3 "~" H 2150 2500 50  0001 C CNN
	1    2150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 2500 1700 2500
$Comp
L lib_symbol:GND #PWR0101
U 1 1 6574D885
P 1800 2700
F 0 "#PWR0101" H 1800 2450 50  0001 C CNN
F 1 "GND" H 1805 2527 50  0000 C CNN
F 2 "" H 1800 2700 50  0001 C CNN
F 3 "" H 1800 2700 50  0001 C CNN
	1    1800 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 2600 1800 2600
Wire Wire Line
	1800 2600 1800 2700
Wire Notes Line
	2150 2500 2250 2600
Text Notes 900  1050 0    150  Italic 30
OFF-PCB BUTTONS
$Comp
L lib_symbol:Conn_01x02_Female J11
U 1 1 657500F8
P 2150 3150
F 0 "J11" H 2050 3250 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 3035 50  0001 L CNN
F 2 "" H 2150 3150 50  0001 C CNN
F 3 "~" H 2150 3150 50  0001 C CNN
	1    2150 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3150 1700 3150
$Comp
L lib_symbol:GND #PWR0139
U 1 1 657500FF
P 1800 3350
F 0 "#PWR0139" H 1800 3100 50  0001 C CNN
F 1 "GND" H 1805 3177 50  0000 C CNN
F 2 "" H 1800 3350 50  0001 C CNN
F 3 "" H 1800 3350 50  0001 C CNN
	1    1800 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3250 1800 3250
Wire Wire Line
	1800 3250 1800 3350
Wire Notes Line
	2150 3150 2250 3250
$Comp
L lib_symbol:Conn_01x02_Female J12
U 1 1 657513D7
P 2150 3800
F 0 "J12" H 2050 3900 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 3685 50  0001 L CNN
F 2 "" H 2150 3800 50  0001 C CNN
F 3 "~" H 2150 3800 50  0001 C CNN
	1    2150 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3800 1700 3800
$Comp
L lib_symbol:GND #PWR0140
U 1 1 657513DE
P 1800 4000
F 0 "#PWR0140" H 1800 3750 50  0001 C CNN
F 1 "GND" H 1805 3827 50  0000 C CNN
F 2 "" H 1800 4000 50  0001 C CNN
F 3 "" H 1800 4000 50  0001 C CNN
	1    1800 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3900 1800 3900
Wire Wire Line
	1800 3900 1800 4000
Wire Notes Line
	2150 3800 2250 3900
$Comp
L lib_symbol:Conn_01x02_Female J13
U 1 1 65752D62
P 2150 4450
F 0 "J13" H 2050 4550 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 4335 50  0001 L CNN
F 2 "" H 2150 4450 50  0001 C CNN
F 3 "~" H 2150 4450 50  0001 C CNN
	1    2150 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 4450 1700 4450
$Comp
L lib_symbol:GND #PWR0158
U 1 1 65752D69
P 1800 4650
F 0 "#PWR0158" H 1800 4400 50  0001 C CNN
F 1 "GND" H 1805 4477 50  0000 C CNN
F 2 "" H 1800 4650 50  0001 C CNN
F 3 "" H 1800 4650 50  0001 C CNN
	1    1800 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 4550 1800 4550
Wire Wire Line
	1800 4550 1800 4650
Wire Notes Line
	2150 4450 2250 4550
$Comp
L lib_symbol:Conn_01x02_Female J14
U 1 1 65755016
P 2150 5100
F 0 "J14" H 2050 5200 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 4985 50  0001 L CNN
F 2 "" H 2150 5100 50  0001 C CNN
F 3 "~" H 2150 5100 50  0001 C CNN
	1    2150 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 5100 1700 5100
$Comp
L lib_symbol:GND #PWR0180
U 1 1 6575501D
P 1800 5300
F 0 "#PWR0180" H 1800 5050 50  0001 C CNN
F 1 "GND" H 1805 5127 50  0000 C CNN
F 2 "" H 1800 5300 50  0001 C CNN
F 3 "" H 1800 5300 50  0001 C CNN
	1    1800 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 5200 1800 5200
Wire Wire Line
	1800 5200 1800 5300
Wire Notes Line
	2150 5100 2250 5200
$Comp
L lib_symbol:Conn_01x02_Female J15
U 1 1 65756C15
P 2150 5750
F 0 "J15" H 2050 5850 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2178 5635 50  0001 L CNN
F 2 "" H 2150 5750 50  0001 C CNN
F 3 "~" H 2150 5750 50  0001 C CNN
	1    2150 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 5750 1700 5750
$Comp
L lib_symbol:GND #PWR0181
U 1 1 65756C1C
P 1800 5950
F 0 "#PWR0181" H 1800 5700 50  0001 C CNN
F 1 "GND" H 1805 5777 50  0000 C CNN
F 2 "" H 1800 5950 50  0001 C CNN
F 3 "" H 1800 5950 50  0001 C CNN
	1    1800 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 5850 1800 5850
Wire Wire Line
	1800 5850 1800 5950
Wire Notes Line
	2150 5750 2250 5850
$Comp
L lib_symbol:Conn_01x02_Female J16
U 1 1 6576F3E9
P 2650 2500
F 0 "J16" H 2550 2600 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 2385 50  0001 L CNN
F 2 "" H 2650 2500 50  0001 C CNN
F 3 "~" H 2650 2500 50  0001 C CNN
	1    2650 2500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 2500 3100 2500
$Comp
L lib_symbol:GND #PWR0182
U 1 1 6576F3F0
P 3000 2700
F 0 "#PWR0182" H 3000 2450 50  0001 C CNN
F 1 "GND" H 3005 2527 50  0000 C CNN
F 2 "" H 3000 2700 50  0001 C CNN
F 3 "" H 3000 2700 50  0001 C CNN
	1    3000 2700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 2600 3000 2600
Wire Wire Line
	3000 2600 3000 2700
Wire Notes Line
	2650 2500 2550 2600
$Comp
L lib_symbol:Conn_01x02_Female J17
U 1 1 6576F3F9
P 2650 3150
F 0 "J17" H 2550 3250 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 3035 50  0001 L CNN
F 2 "" H 2650 3150 50  0001 C CNN
F 3 "~" H 2650 3150 50  0001 C CNN
	1    2650 3150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 3150 3100 3150
$Comp
L lib_symbol:GND #PWR0183
U 1 1 6576F400
P 3000 3350
F 0 "#PWR0183" H 3000 3100 50  0001 C CNN
F 1 "GND" H 3005 3177 50  0000 C CNN
F 2 "" H 3000 3350 50  0001 C CNN
F 3 "" H 3000 3350 50  0001 C CNN
	1    3000 3350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 3250 3000 3250
Wire Wire Line
	3000 3250 3000 3350
Wire Notes Line
	2650 3150 2550 3250
$Comp
L lib_symbol:Conn_01x02_Female J18
U 1 1 6576F409
P 2650 3800
F 0 "J18" H 2550 3900 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 3685 50  0001 L CNN
F 2 "" H 2650 3800 50  0001 C CNN
F 3 "~" H 2650 3800 50  0001 C CNN
	1    2650 3800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 3800 3100 3800
$Comp
L lib_symbol:GND #PWR0184
U 1 1 6576F410
P 3000 4000
F 0 "#PWR0184" H 3000 3750 50  0001 C CNN
F 1 "GND" H 3005 3827 50  0000 C CNN
F 2 "" H 3000 4000 50  0001 C CNN
F 3 "" H 3000 4000 50  0001 C CNN
	1    3000 4000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 3900 3000 3900
Wire Wire Line
	3000 3900 3000 4000
Wire Notes Line
	2650 3800 2550 3900
$Comp
L lib_symbol:Conn_01x02_Female J19
U 1 1 6576F419
P 2650 4450
F 0 "J19" H 2550 4550 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 4335 50  0001 L CNN
F 2 "" H 2650 4450 50  0001 C CNN
F 3 "~" H 2650 4450 50  0001 C CNN
	1    2650 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 4450 3100 4450
$Comp
L lib_symbol:GND #PWR0185
U 1 1 6576F420
P 3000 4650
F 0 "#PWR0185" H 3000 4400 50  0001 C CNN
F 1 "GND" H 3005 4477 50  0000 C CNN
F 2 "" H 3000 4650 50  0001 C CNN
F 3 "" H 3000 4650 50  0001 C CNN
	1    3000 4650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 4550 3000 4550
Wire Wire Line
	3000 4550 3000 4650
Wire Notes Line
	2650 4450 2550 4550
$Comp
L lib_symbol:Conn_01x02_Female J20
U 1 1 6576F429
P 2650 5100
F 0 "J20" H 2550 5200 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 4985 50  0001 L CNN
F 2 "" H 2650 5100 50  0001 C CNN
F 3 "~" H 2650 5100 50  0001 C CNN
	1    2650 5100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 5100 3100 5100
$Comp
L lib_symbol:GND #PWR0186
U 1 1 6576F430
P 3000 5300
F 0 "#PWR0186" H 3000 5050 50  0001 C CNN
F 1 "GND" H 3005 5127 50  0000 C CNN
F 2 "" H 3000 5300 50  0001 C CNN
F 3 "" H 3000 5300 50  0001 C CNN
	1    3000 5300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 5200 3000 5200
Wire Wire Line
	3000 5200 3000 5300
Wire Notes Line
	2650 5100 2550 5200
$Comp
L lib_symbol:Conn_01x02_Female J21
U 1 1 6576F439
P 2650 5750
F 0 "J21" H 2550 5850 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2678 5635 50  0001 L CNN
F 2 "" H 2650 5750 50  0001 C CNN
F 3 "~" H 2650 5750 50  0001 C CNN
	1    2650 5750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 5750 3100 5750
$Comp
L lib_symbol:GND #PWR0187
U 1 1 6576F440
P 3000 5950
F 0 "#PWR0187" H 3000 5700 50  0001 C CNN
F 1 "GND" H 3005 5777 50  0000 C CNN
F 2 "" H 3000 5950 50  0001 C CNN
F 3 "" H 3000 5950 50  0001 C CNN
	1    3000 5950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2850 5850 3000 5850
Wire Wire Line
	3000 5850 3000 5950
Wire Notes Line
	2650 5750 2550 5850
$EndSCHEMATC
