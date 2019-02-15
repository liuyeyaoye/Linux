
```c
Ò». features
1. power saving modes
	sleep mode
	idle mode
	
2. CABC: content adaptive brightness control£¬to reduce power consumption of the luminance(ÁÁ¶È)
	DGC£ºdigital gamma correction
	GIP£ºgate control signals to gate dirver in panel
	OSC£ºÕñµ´Æ÷
	
 ¶þ. DSI system interface
 1.  general description
lansel		IM[2:0]		Page4_R00h  MIPI_LANE_SEL			D0P/N Pin		D1P/N Pin		CLKP/N Pin		D2P/N Pin		D3P/N Pin
0			0 0 0		1									D3P/N  			D2P/N  			CLKP/N  			D1P/N  			D0P/N
	
2.	communication
(1)	Low level communication is done on the interface level
(2)	High level communication is done on the packet level

3.	interface level communication
(1)	communication mode
low power		and		high speed
	
(2)
Lane Pair State Code						Line DC Voltage Levels						High Speed (HS)  					Low Power
										DATA_P  	DATA_N  							Burst Mode  						Control Mode  			Escape Mode
HS-0  									Low (HS)  	High (HS)  					Differential ¨C 0  					Note 1  					Note1
HS-1 									High (HS)  	Low (HS)  					Differential ¨C 1  					Note 1  					Note 1
LP-00  									Low (LP)  	Low (LP)  					Not Defined  						Bridge  					Space
LP-01  									Low (LP)  	High (LP)  					Not Defined  						HS ¨C Request  			Mark - 0
LP-10  									High (LP)  	Low (LP)  					Not Defined  						LP - Request  				Mark - 1
LP-11  									High (LP)  	High (LP)  					Not Defined  						Stop  					Note 2

4.	DSI clock lanes	
(0)	three modes:
		Low Power Mode (LPM)		Ultra-Low Power Mode (ULPM)		High-Speed Clock Mode (HSCM)

(1)	CLKP/N lanes can be driven into three different power modes: Low Power Mode (LPM), Ultra-Low Power Mode(ULPM) and High Speed Clock Mode (HSCM).

(2)	Clock lane are in the single ended mode (LP = Low Power) when entering or leaving Low Power Mode (LPM) , Ultra-Low Power Mode (ULPM), High Speed Clock Mode (HSCM).

(3)	the burst of the HS clock consist of£º
		even number of transmission
		start state is HS-0
		end state is HS-0
		
(4)	High Speed Clock Burst
High Speed Mode, Enable Rx Line Termination
Low Power Mode, Disable Rx Line Termination

in the HS EOT £º
If the DnP/N last load bit is HS-0, the transmitter changes from HS-0 to HS-1.
If the DnP/N last load bit is HS-1, the transmitter changes from HS-1 to HS-0.

5.	DSI data Lane
(1)	three modes£º
		Escape Mode (Only D0P/N data lane is used)
		High-Speed Data Transmission (all data lanes are used)
		Bus Turnaround(BTA) Request (Only D0P/N data lane are used)

Mode  							Entering Mode Sequence  								Leaving Mode Sequence
Escape Mode  					LP-11 ¡ª LP-10 ¡ª LP-00 ¡ª LP-01 ¡ª LP-00  				LP-00 ¡ª LP-10 ¡ª LP-11 (Mark-1)
High-Speed Data Transmission  		LP-11 ¡ª LP-01 ¡ª LP-00 ¡ª HS-0  							(HS-0 or HS-1 ) ¡ª LP-11
Bus Turnaround Request  			LP-11 ¡ª LP-10 ¡ª LP-00 ¡ª LP-10 ¡ª LP-00  				Hi-Z

(3) escape mode:
Send ¡°Low-Power Data Transmission¡± (LPDT) from the MCU to the display module,
Drive data lanes to ¡°Ultra-Low Power State¡± (ULPS),
Indicate ¡°Remote Application Reset¡± (RAR), which can reset the display module,
Indicate ¡°Acknowledge¡± (ACK), which is used to transmit a non-error event from the display module to the MCU.

(4)	High-Speed Data Transmission
	
(5)	Burst of the High-Speed Data Transmission (HSDT)

(6)	Bus Turnaround(BTA) Request
		from the MCU to the display module£ºthe display module starts to control D0P/N data lanes and the MCU stops to control D0P/N data lanes (= High-Z)

6.	Packet Level Communication
(1)	 the Least Significant (LS) Byte of the information is sent first £¬ the Most Significant (MS) Byte is sent last

```


