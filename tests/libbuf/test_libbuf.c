#include <stdio.h>

#include <atf-c.h>

#include <sys/queue.h>

#include "libbuf/buf.h"

ATF_TC_WITHOUT_HEAD(test_test);
ATF_TC_BODY(test_test, tc)
{
}

ATF_TC_WITHOUT_HEAD(test_create_destroy);
ATF_TC_BODY(test_create_destroy, tc)
{
	struct buf *b;

	b = buf_create(1024);
	ATF_REQUIRE(b != NULL);

	buf_free(b);
	/* XXX no way to check buf_free works? */
}

static const char *test_strings[] = {
    "abc\r\n",
    "abcdef\n",
    "KB7STN-3>APNW01,TCPIP*,qAS,KB7STN::KB7STN-3 :EQNS.0,10,0,0,10,0,0,1,0,0,0,KB7STN-3>W3ISZ-1>APRX28,TCPIP*,qAC,T2USANW:!3535.01NI11740.54W#SARC repeater 147.000 pl 107.2\r\n",
    "W7GC-S>APJIO4,TCPIP*,qAC,W7GC-GS:;W7GC   C *011443z4512.86ND12345.38Wa 2m Voice 147.39 +0.600 MHz\r\n",
    "SUNSET>APRX29,TCPIP*,qAC,APRSFI-I2:!3745.60NI12229.85W#W2GMD Outer Sunset, SF IGate/Digipeater http://w2gmd.org\r\n",
    "WD8CIK-10>APT314,WIDE1-1,WIDE2-1,qAR,KF6NMZ-4:!3409.45N/11834.75W>297/010!w(o!\r\n",
    "K6TZ-9>APRX29,TCPIP*,qAC,T2QUEBEC:;146.79-LV*111111z3424.33N/11942.92WrT131 Net W 730 Mtg3rdF\r\n",
    "KD7IIW-6>APWW10,TCPIP*,qAC,T2PARIS:@032146h4145.77N/11151.73WlHarvestMaster\r\n",
    "N5PSM-5>APOTC1,WB6TMS-5*,WIDE1*,BKELEY*,WIDE2-1,qAR,KC6SSM-5:!/;&sZ/MXmjl3G/A=000220 12.2V 25C\r\n",
    "KG6YVD-11>APRS,HAYDEN*,WIDE2,qAR,KD6GHX:@020142z3406.73N/11424.60W_000/000g000t068r000p000P000h61b10075.DsVP\r\n",
    "HEBOWX>APN390,WIDE2-1,qAR,W7IG:@033303z4512.89NS12345.29W_288/010g016t041r003p044P017h00b10102.WxDg\r\n",
    "KJ7UK-10>S8USPQ,WIDE1-1,WIDE2-1,qAR,KJ6IX-5:`/H8l!?>/'\"D1}MY JEEP !|3\r\n",
    "W6LLL-15>APTW14,K7FED-1*,WIDE2-1,qAR,W6SRR-3:_11010905c079s003g004t056rW6LLL-15>APTW14,K7FED-1*,WN3EG>BEACON,KE7ZXE-2*,qAO,AK9E-1:T#233,133,082,033,016,075,00010001\r\n",
    "W6PKT-5>APWW10,TCPIP*,qAC,T2SJC:=3919.53NI12316.42W&\r\n",
    "WD0DAJ>S2SYYQ,WIDE1-1,WIDE2-2,qAS,K6NYH:`*7ol\"K>/'\"4b}MT-RTG|):%R'a|!w0.!|3\r\n",
    "KA7EZO-15>APSAR,TCPIP*,qAC,WE7U-IE:=3931.88N/11128.06Wc/A=005000[:MPBase\r\n",
    "K6IFR-B>APJI40,TCPIP*,qAC,K6IFR-GS:!3352.19ND11625.77W&RNG0065 440 Voice 447.140 -5.00 MHz\r\n",
    "K6IFR-B>APJI40,TCPIP*,qAC,K6IFR-GS:<IGATE,MSG_CNT=0,LOC_CNT=1\r\n",
    "KK6FYJ>DD4SRP,KELLER,WIDE1*,WK6IFR-B>APJI40,TCPIP*,qAC,K6IFR-GSKWX6HNX-11>APRS,WIDE1-1,qAR,KI6PIO-7:@020323z3636.25N/11843.98W_000/000g000t040r000p000P000h69b10160.DsVP\r\n",
    "DOUGCO>APN390,qAR,WA6KHG:!4311.74NS12307.04W#PHG3850/W2,ORn-N, LANE MT., DOUGLAS COUNTY, ROSEBURG, OR  AE7ER\r\n",
    "KC7ZNH-2>T0SVTS,WIDE1-1,WIDE2-1,qAR,KB7STN-3:`'U=l!;>/'\"Bj}|+y%L'[|!wH8!|3\r\n",
    "K7OCR-10>APWW10,TCPIP*,qAC,T2SWEDEN:<IGATE,MSG_CNT=0,LOC_CNT=2,DIR_CNT=1,RF_CNT=6,R/O!\r\n",
    "W7BOZ>APRX29,TCPIP*,qAC,THIRD:T#272,16.5,0.5,75.0,0.0,3.0,00000000\r\n"
    "KB1LQC-1>GPSFDY,qAS,KB1LQC:!3403.36N\\11814.78W#.../000/A=000100Faraday Access Point|$4;u8s9A!!!?!!|\r\n",
    "K6ECM-2>APRS,TCPIP*,qAC,T2VAN:=3537.60N/11742.84W-PHG0000/AllStar Node 40631\r\n",
    "EDOM>BEACON,qAR,AA6HF:EDOM is W6CDF Palm Springs low level digipeater\r\n",
    "WX7FGZ-1>ARPS,CANYON*,WIDE1*,BWMTN*,WIDE2-1,qAR,N6DI-1:$ULTW0073002502DD0003----0000000000000142013004C400000027\r\n",
    "W7MOT-3>APN382,WIDE2-2,qAS,K0OTZ-7:!3334.67NS11234.43W#PHG5880/White Tanks, AZ VHF Digi, 3880 ft ASL\r\n",
    "KF6ILA>GPSHW,KF6ILA-10*,qAR,WD6DRI-1:@091642z3303.70N/11634.43W_241/003g006t047r000p000P000h96b10138L000.DsVP\r\n",
    "WX6HNX-14>APRS,NR6G-9*,WIDE2-1,qAR,NR6G-7:@020313z3503.50N/11905.05W_000/000g000t057r000p000P000h75b10131.DsVP\r\n",
    "K6CPP>APRS,TCPIP*,qAC,T2EDM:;ER-K6CPP *111111z3403.40NE11749.26W0445.580MHz T156 R04m Contact grshiery@cpp.edu for info\r\n",
    "K7FED-9>APT311,WIDE1-1,WIDE2-1,qAR,K6SJC-1:!3743.24N/12124.97Wk358/022/A=000000/listening 146.520 MicroTrak8000\r\n",
    "PATSY>APN382,WIDE1-1,qAo,EUGENE:@002804z4436.81N112339.02W_000/000g000t077r000p000P000h00b......WxDg\r\n",
    "K6YG>APRS,TCPIP*,qAC,T2ONTARIO:@020321z3538.18N/11742.14W_354/018g026t064r342p389h38b10067.wview_5_21_7\r\n",
    "KE6AFE-2>APU25N,TCPIP*,qAC,T2EDM:;TFCSCRUZ *020321z3653.94N\\12200.92W?14 In 10 Minutes\r\n",
    "W7IG>APWW10,TCPIP*,qAC,T2USANW:@032157h4507.07N/12323.48WyAPRS-IS\r\n",
    "XE2SI-9>APT311,WIDE1-1,WIDE2-1,qAR,XE2GF-10:/032159h3230.48N/11659.73W(109/000/A=000223\r\n",
    "XE2SI-9>APT311,WIDE1-1,WIDE2-1,qAR,XE2GF-10:>Paseando \r\n",
    NULL
};

/*
 * Append three lines, ensure we can consume those lines.
 */
ATF_TC_WITHOUT_HEAD(test_append_line_1);
ATF_TC_BODY(test_append_line_1, tc)
{
	char buf[1024];
	struct buf *b;
	int i, r, l;

	b = buf_create(65536);
	ATF_REQUIRE(b != NULL);

	for (i = 0; test_strings[i] != NULL; i++) {
		l = strlen(test_strings[i]);
		printf("%s: testing string %s\r\n", __func__, test_strings[i]);
		printf("%s: strlen=%d, buf len=%d\n", __func__, l, b->len);
		r = buf_append(b, test_strings[i], l);
		printf("%s: append len=%d\n", __func__, r);
		ATF_REQUIRE(r == l);

		r = buf_gets(b, buf, 1024);
		printf("%s: gets len=%d\n", __func__, r);
		ATF_REQUIRE(r == l);

		ATF_REQUIRE(memcmp(buf, test_strings[i], l) == 0);
		printf("==\n");
	}

	buf_free(b);
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, test_test);
	ATF_TP_ADD_TC(tp, test_create_destroy);
	ATF_TP_ADD_TC(tp, test_append_line_1);
	return (atf_no_error());
}
