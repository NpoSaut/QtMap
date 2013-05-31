#include <iostream>
#include <math.h>

#include <QApplication>
#include <QtConcurrentRun>
#include <QTextStream>
#include <QTextCodec>
#include <qmlapplicationviewer.h>

#include "systemstateviewmodel.h"
#include "electroincmap.h"

#ifdef WITH_CAN
#include "iodrv/iodrv.h"
#include "iodrv/emapcanemitter.h"
#endif


SystemStateViewModel *systemState ;
Navigation::ElectroincMap* elMap;

#ifdef WITH_CAN
iodrv* iodriver;
SpeedAgregator* speedAgregator;
rmp_key_handler* rmp_key_hdlr;
EMapCanEmitter* emapCanEmitter;
#endif



/*void getSpeed (double* speed)
{
    systemState->setSpeed( int(*speed) );
}

void getSpeedLimits (int* val)
{
    systemState->setSpeedRestriction(*val);
}

void getLights (int* code)
{
    systemState->setLight(*code-1);
}

void getAlsn (int* code)
{
    if (*code == 0)
        systemState->setAlsnFreq(50);
    else if (*code == 1)
        systemState->setAlsnFreq(75);
    else if (*code == 2)
        systemState->setAlsnFreq(50);
    else if (*code == 3)
        systemState->setAlsnFreq(25);
}

void getMilage (int* val)
{
    systemState->setMilage( (*val)/1000 );
}

void getGps (double* lat, double* lon)
{
    systemState->setLongitude(*lon);
    systemState->setLatitude(*lat);
}

void getDateTime (int* h, int* m, int* s)
{
    QString date = QString("%1:%2:%3").arg(*h).arg(*m).arg(*s);
    systemState->setTime(date);
}*/

#ifdef WITH_CAN
//extern void aFunction();
//QFuture<void> future = QtConcurrent::run(aFunction);
/*void getParamsFromCan ()
{
    sktcanl_init();
    sktcanl_set_callbacks(getSpeed, getSpeedLimits, NULL, NULL, getLights, getAlsn, getMilage, NULL, NULL, getGps, getDateTime);

    while (true)
    {
        sktcanl_read_can_msg();
    }
}*/
#endif


double coords[][3] = {
    { 55.624846, 37.677687, 0 },
    { 55.624452, 37.676990, 62 },
    { 55.624022, 37.676475, 120 },
    { 55.623828, 37.676336, 143 },
    { 55.623653, 37.676089, 168 },
    { 55.623380, 37.675703, 207 },
    { 55.623126, 37.675467, 239 },
    { 55.622889, 37.675198, 270 },
    { 55.622671, 37.674952, 299 },
    { 55.622441, 37.674673, 330 },
    { 55.622163, 37.674383, 365 },
    { 55.621969, 37.674297, 388 },
    { 55.621751, 37.674040, 417 },
    { 55.621490, 37.673739, 451 },
    { 55.621193, 37.673460, 489 },
    { 55.621018, 37.673289, 511 },
    { 55.620787, 37.673106, 539 },
    { 55.620563, 37.672806, 570 },
    { 55.620369, 37.672666, 594 },
    { 55.620157, 37.672516, 619 },
    { 55.619909, 37.672237, 652 },
    { 55.619685, 37.671990, 681 },
    { 55.619545, 37.671862, 699 },
    { 55.619164, 37.671508, 746 },
    { 55.619012, 37.671368, 766 },
    { 55.618843, 37.671154, 789 },
    { 55.618649, 37.670971, 813 },
    { 55.618497, 37.670767, 834 },
    { 55.618116, 37.670220, 889 },
    { 55.617958, 37.669941, 914 },
    { 55.617776, 37.669705, 939 },
    { 55.617661, 37.669480, 958 },
    { 55.617570, 37.669383, 970 },
    { 55.617443, 37.669104, 992 },
    { 55.617316, 37.668911, 1011 },
    { 55.617152, 37.668675, 1034 },
    { 55.617092, 37.668504, 1047 },
    { 55.616977, 37.668203, 1070 },
    { 55.616849, 37.667913, 1093 },
    { 55.616734, 37.667559, 1118 },
    { 55.616619, 37.667195, 1145 },
    { 55.616492, 37.666969, 1165 },
    { 55.616346, 37.666583, 1194 },
    { 55.616292, 37.666197, 1219 },
    { 55.616128, 37.665821, 1249 },
    { 55.615983, 37.665446, 1277 },
    { 55.615898, 37.664963, 1309 },
    { 55.615874, 37.664459, 1341 },
    { 55.615795, 37.664266, 1356 },
    { 55.615753, 37.664008, 1372 },
    { 55.615674, 37.663643, 1397 },
    { 55.615625, 37.663246, 1423 },
    { 55.615589, 37.663032, 1437 },
    { 55.615534, 37.662742, 1456 },
    { 55.615504, 37.662560, 1468 },
    { 55.615419, 37.662141, 1496 },
    { 55.615395, 37.661927, 1509 },
    { 55.615335, 37.661562, 1533 },
    { 55.615256, 37.661240, 1555 },
    { 55.615195, 37.660929, 1576 },
    { 55.615135, 37.660629, 1596 },
    { 55.615086, 37.660339, 1615 },
    { 55.615038, 37.660167, 1627 },
    { 55.614995, 37.659845, 1648 },
    { 55.614916, 37.659513, 1670 },
    { 55.614838, 37.659148, 1695 },
    { 55.614741, 37.658783, 1720 },
    { 55.614680, 37.658644, 1731 },
    { 55.614644, 37.658451, 1744 },
    { 55.614553, 37.658172, 1764 },
    { 55.614517, 37.658021, 1775 },
    { 55.614420, 37.657710, 1797 },
    { 55.614298, 37.657335, 1824 },
    { 55.614250, 37.657034, 1844 },
    { 55.614177, 37.656927, 1854 },
    { 55.614050, 37.656637, 1877 },
    { 55.613929, 37.656358, 1899 },
    { 55.613832, 37.656208, 1914 },
    { 55.613759, 37.656015, 1928 },
    { 55.613674, 37.655618, 1955 },
    { 55.613547, 37.655371, 1976 },
    { 55.613456, 37.655189, 1991 },
    { 55.613329, 37.654846, 2017 },
    { 55.613244, 37.654717, 2029 },
    { 55.613135, 37.654470, 2049 },
    { 55.612990, 37.654191, 2073 },
    { 55.612893, 37.653923, 2093 },
    { 55.612796, 37.653816, 2106 },
    { 55.612735, 37.653623, 2119 },
    { 55.612632, 37.653430, 2136 },
    { 55.612547, 37.653322, 2148 },
    { 55.612438, 37.653161, 2164 },
    { 55.612341, 37.653054, 2176 },
    { 55.612232, 37.652882, 2193 },
    { 55.612178, 37.652764, 2202 },
    { 55.612093, 37.652636, 2214 },
    { 55.612020, 37.652496, 2226 },
    { 55.611935, 37.652389, 2238 },
    { 55.611838, 37.652282, 2251 },
    { 55.611747, 37.652142, 2264 },
    { 55.611681, 37.652078, 2273 },
    { 55.611608, 37.651970, 2283 },
    { 55.611529, 37.651842, 2295 },
    { 55.611487, 37.651734, 2303 },
    { 55.611426, 37.651670, 2311 },
    { 55.611372, 37.651616, 2318 },
    { 55.611329, 37.651509, 2326 },
    { 55.611269, 37.651445, 2334 },
    { 55.611214, 37.651380, 2341 },
    { 55.611117, 37.651273, 2354 },
    { 55.611038, 37.651176, 2365 },
    { 55.611008, 37.651134, 2369 },
    { 55.610941, 37.651016, 2380 },
    { 55.610875, 37.650951, 2388 },
    { 55.610845, 37.650919, 2392 },
    { 55.610778, 37.650855, 2400 },
    { 55.610717, 37.650812, 2408 },
    { 55.610645, 37.650715, 2418 },
    { 55.610554, 37.650672, 2428 },
    { 55.610499, 37.650554, 2438 },
    { 55.610432, 37.650479, 2447 },
    { 55.610366, 37.650447, 2454 },
    { 55.610305, 37.650447, 2461 },
    { 55.610239, 37.650393, 2469 },
    { 55.610202, 37.650318, 2475 },
    { 55.610142, 37.650222, 2484 },
    { 55.610093, 37.650243, 2490 },
    { 55.610014, 37.650157, 2500 },
    { 55.609966, 37.650093, 2507 },
    { 55.609887, 37.649964, 2519 },
    { 55.609814, 37.649964, 2527 },
    { 55.609766, 37.649932, 2533 },
    { 55.609693, 37.649868, 2542 },
    { 55.609626, 37.649846, 2549 },
    { 55.609530, 37.649771, 2561 },
    { 55.609451, 37.649685, 2571 },
    { 55.609384, 37.649621, 2580 },
    { 55.609323, 37.649546, 2588 },
    { 55.609251, 37.649481, 2597 },
    { 55.609154, 37.649471, 2608 },
    { 55.609075, 37.649417, 2617 },
    { 55.609008, 37.649385, 2625 },
    { 55.608924, 37.649342, 2635 },
    { 55.608845, 37.649299, 2644 },
    { 55.608778, 37.649267, 2652 },
    { 55.608693, 37.649235, 2661 },
    { 55.608602, 37.649181, 2672 },
    { 55.608499, 37.649095, 2685 },
    { 55.608414, 37.649063, 2694 },
    { 55.608305, 37.648977, 2707 },
    { 55.608196, 37.648998, 2720 },
    { 55.608069, 37.648870, 2736 },
    { 55.607978, 37.648870, 2746 },
    { 55.607948, 37.648848, 2750 },
    { 55.607881, 37.648848, 2757 },
    { 55.607815, 37.648816, 2765 },
    { 55.607784, 37.648784, 2769 },
    { 55.607705, 37.648827, 2778 },
    { 55.607645, 37.648762, 2786 },
    { 55.607578, 37.648741, 2793 },
    { 55.607518, 37.648741, 2800 },
    { 55.607408, 37.648666, 2813 },
    { 55.607348, 37.648623, 2820 },
    { 55.607251, 37.648623, 2831 },
    { 55.607215, 37.648580, 2836 },
    { 55.607124, 37.648591, 2846 },
    { 55.607124, 37.648591, 2846 },
    { 55.607063, 37.648569, 2853 },
    { 55.606972, 37.648473, 2865 },
    { 55.606881, 37.648537, 2876 },
    { 55.606809, 37.648505, 2884 },
    { 55.606705, 37.648484, 2895 },
    { 55.606627, 37.648484, 2904 },
    { 55.606548, 37.648526, 2913 },
    { 55.606499, 37.648505, 2919 },
    { 55.606402, 37.648451, 2930 },
    { 55.606299, 37.648462, 2942 },
    { 55.606209, 37.648441, 2952 },
    { 55.606099, 37.648441, 2964 },
    { 55.606009, 37.648451, 2974 },
    { 55.605899, 37.648430, 2986 },
    { 55.605742, 37.648451, 3004 },
    { 55.605651, 37.648451, 3014 },
    { 55.605578, 37.648451, 3022 },
    { 55.605506, 37.648451, 3030 },
    { 55.605427, 37.648462, 3039 },
    { 55.605312, 37.648441, 3052 },
    { 55.605178, 37.648473, 3067 },
    { 55.605075, 37.648494, 3078 },
    { 55.604996, 37.648505, 3087 },
    { 55.604930, 37.648559, 3095 },
    { 55.604851, 37.648537, 3104 },
    { 55.604772, 37.648537, 3113 },
    { 55.604705, 37.648548, 3120 },
    { 55.604633, 37.648580, 3129 },
    { 55.604512, 37.648612, 3142 },
    { 55.604409, 37.648644, 3154 },
    { 55.604318, 37.648687, 3164 },
    { 55.604209, 37.648720, 3177 },
    { 55.604105, 37.648698, 3188 },
    { 55.604027, 37.648687, 3197 },
    { 55.603936, 37.648752, 3208 },
    { 55.603875, 37.648773, 3215 },
    { 55.603796, 37.648827, 3224 },
    { 55.603730, 37.648827, 3232 },
    { 55.603639, 37.648870, 3242 },
    { 55.603560, 37.648880, 3251 },
    { 55.603439, 37.648913, 3264 },
    { 55.603384, 37.648977, 3272 },
    { 55.603305, 37.649020, 3281 },
    { 55.603221, 37.649084, 3291 },
    { 55.603160, 37.649106, 3298 },
    { 55.603099, 37.649149, 3305 },
    { 55.603021, 37.649149, 3314 },
    { 55.602899, 37.649224, 3328 },
    { 55.602815, 37.649299, 3339 },
    { 55.602681, 37.649353, 3354 },
    { 55.602608, 37.649395, 3363 },
    { 55.602518, 37.649471, 3374 },
    { 55.602396, 37.649513, 3388 },
    { 55.602348, 37.649589, 3395 },
    { 55.602275, 37.649621, 3403 },
    { 55.602202, 37.649664, 3412 },
    { 55.602111, 37.649739, 3423 },
    { 55.602002, 37.649771, 3435 },
    { 55.601911, 37.649792, 3445 },
    { 55.601851, 37.649878, 3454 },
    { 55.601681, 37.650039, 3475 },
    { 55.601645, 37.650050, 3479 },
    { 55.601572, 37.650114, 3488 },
    { 55.601517, 37.650157, 3495 },
    { 55.601463, 37.650254, 3504 },
    { 55.601427, 37.650275, 3508 },
    { 55.601348, 37.650350, 3518 },
    { 55.601299, 37.650372, 3524 },
    { 55.601214, 37.650458, 3534 },
    { 55.601172, 37.650511, 3540 },
    { 55.601123, 37.650543, 3546 },
    { 55.601008, 37.650608, 3559 },
    { 55.600905, 37.650683, 3572 },
    { 55.600814, 37.650779, 3584 },
    { 55.600723, 37.650822, 3594 },
    { 55.600651, 37.650865, 3602 },
    { 55.600578, 37.650930, 3612 },
    { 55.600523, 37.650973, 3618 },
    { 55.600469, 37.651005, 3625 },
    { 55.600402, 37.651058, 3633 },
    { 55.600360, 37.651101, 3638 },
    { 55.600257, 37.651176, 3651 },
    { 55.600208, 37.651209, 3656 },
    { 55.600111, 37.651294, 3668 },
    { 55.600051, 37.651337, 3676 },
    { 55.599948, 37.651434, 3689 },
    { 55.599863, 37.651477, 3698 },
    { 55.599796, 37.651552, 3707 },
    { 55.599729, 37.651649, 3717 },
    { 55.599590, 37.651756, 3734 },
    { 55.599626, 37.651552, 3747 },
    { 55.599487, 37.651799, 3769 },
    { 55.599378, 37.651917, 3783 },
    { 55.599269, 37.651981, 3796 },
    { 55.599105, 37.652131, 3817 },
    { 55.599044, 37.652196, 3824 },
    { 55.599026, 37.652389, 3837 },
    { 55.598899, 37.652271, 3853 },
    { 55.598838, 37.652324, 3860 },
    { 55.598699, 37.652367, 3876 },
    { 55.598638, 37.652389, 3883 },
    { 55.598547, 37.652110, 3903 },
    { 55.598553, 37.652507, 3928 },
    { 55.598450, 37.652571, 3940 },
    { 55.598378, 37.652657, 3950 },
    { 55.598287, 37.652721, 3961 },
    { 55.598226, 37.652743, 3968 },
    { 55.598190, 37.652786, 3972 },
    { 55.598111, 37.652882, 3983 },
    { 55.598111, 37.652678, 3996 },
    { 55.598032, 37.652925, 4014 },
    { 55.597929, 37.653022, 4027 },
    { 55.597844, 37.652936, 4038 },
    { 55.597699, 37.653108, 4057 },
    { 55.597632, 37.652968, 4069 },
    { 55.597541, 37.653151, 4084 },
    { 55.597493, 37.653097, 4090 },
    { 55.597414, 37.653226, 4102 },
    { 55.597383, 37.653236, 4106 },
    { 55.597256, 37.653204, 4120 },
    { 55.597117, 37.653215, 4135 },
    { 55.597014, 37.653311, 4148 },
    { 55.596862, 37.653408, 4166 },
    { 55.596674, 37.653537, 4189 },
    { 55.596541, 37.653644, 4205 },
    { 55.596462, 37.653623, 4214 },
    { 55.596365, 37.653687, 4225 },
    { 55.596298, 37.653741, 4234 },
    { 55.596256, 37.653805, 4240 },
    { 55.596165, 37.653784, 4250 },
    { 55.596092, 37.653848, 4259 },
    { 55.595989, 37.653902, 4271 },
    { 55.595813, 37.654063, 4293 },
    { 55.595735, 37.654105, 4302 },
    { 55.595644, 37.654213, 4314 },
    { 55.595529, 37.654288, 4328 },
    { 55.595419, 37.654352, 4341 },
    { 55.595286, 37.654384, 4356 },
    { 55.595147, 37.654459, 4372 },
    { 55.595007, 37.654245, 4392 },
    { 55.594928, 37.654513, 4411 },
    { 55.594843, 37.654599, 4422 },
    { 55.595062, 37.654567, 4447 },
    { 55.594813, 37.654620, 4475 },
    { 55.594692, 37.654567, 4488 },
    { 55.594607, 37.654610, 4498 },
    { 55.594462, 37.654674, 4515 },
    { 55.594365, 37.654717, 4526 },
    { 55.594304, 37.654738, 4533 },
    { 55.594213, 37.654653, 4544 },
    { 55.594146, 37.654738, 4553 },
    { 55.594061, 37.654728, 4563 },
    { 55.593940, 37.654771, 4577 },
    { 55.593861, 37.654771, 4585 },
    { 55.593819, 37.654781, 4590 },
    { 55.593734, 37.654824, 4600 },
    { 55.593631, 37.654846, 4612 },
    { 55.593546, 37.654878, 4621 },
    { 55.593473, 37.654889, 4629 },
    { 55.593395, 37.654932, 4638 },
    { 55.593292, 37.655007, 4651 },
    { 55.593231, 37.655060, 4658 },
    { 55.593128, 37.655157, 4671 },
    { 55.593019, 37.655243, 4685 },
    { 55.592934, 37.655275, 4694 },
    { 55.592879, 37.655339, 4702 },
    { 55.592794, 37.655371, 4711 },
    { 55.592716, 37.655436, 4721 },
    { 55.592631, 37.655436, 4730 },
    { 55.592558, 37.655489, 4739 },
    { 55.592443, 37.655500, 4752 },
    { 55.592364, 37.655522, 4761 },
    { 55.592291, 37.655522, 4769 },
    { 55.592218, 37.655554, 4777 },
    { 55.592182, 37.655532, 4782 },
    { 55.592109, 37.655629, 4792 },
    { 55.592024, 37.655650, 4801 },
    { 55.591964, 37.655683, 4808 },
    { 55.591897, 37.655715, 4816 },
    { 55.591836, 37.655629, 4825 },
    { 55.591770, 37.655747, 4835 },
    { 55.591649, 37.655801, 4849 },
    { 55.591588, 37.655854, 4856 },
    { 55.591491, 37.655865, 4867 },
    { 55.591448, 37.655908, 4873 },
    { 55.591364, 37.655983, 4883 },
    { 55.591291, 37.656015, 4892 },
    { 55.591285, 37.655865, 4901 },
    { 55.591285, 37.655983, 4908 },
    { 55.591224, 37.656101, 4918 },
    { 55.591067, 37.656069, 4936 },
    { 55.590957, 37.656155, 4949 },
    { 55.590860, 37.656240, 4961 },
    { 55.590733, 37.656219, 4976 },
    { 55.590642, 37.656358, 4989 },
    { 55.590636, 37.656283, 4994 },
    { 55.590557, 37.656369, 5004 },
    { 55.590484, 37.656401, 5012 },
    { 55.590400, 37.656434, 5022 },
    { 55.590351, 37.656487, 5028 },
    { 55.590284, 37.656476, 5036 },
    { 55.590236, 37.656530, 5042 },
    { 55.590181, 37.656584, 5049 },
    { 55.590054, 37.656595, 5063 },
    { 55.589969, 37.656584, 5073 },
    { 55.589848, 37.656702, 5088 },
    { 55.589763, 37.656766, 5098 },
    { 55.589684, 37.656777, 5107 },
    { 55.589617, 37.656766, 5115 },
    { 55.589484, 37.656809, 5130 },
    { 55.589430, 37.656831, 5136 },
    { 55.589326, 37.656884, 5148 },
    { 55.589266, 37.656820, 5156 },
    { 55.589163, 37.656970, 5171 },
    { 55.589078, 37.656938, 5180 },
    { 55.588963, 37.656991, 5193 },
    { 55.588920, 37.657045, 5199 },
    { 55.588835, 37.657056, 5209 },
    { 55.588757, 37.657067, 5218 },
    { 55.588653, 37.657131, 5230 },
    { 55.588569, 37.657163, 5239 },
    { 55.588429, 37.657324, 5258 },
    { 55.588375, 37.657324, 5264 },
    { 55.588284, 37.657356, 5274 },
    { 55.588223, 37.657388, 5281 },
    { 55.588126, 37.657410, 5292 },
    { 55.588096, 37.657410, 5295 },
    { 55.588023, 37.657474, 5305 },
    { 55.587871, 37.657560, 5322 },
    { 55.587841, 37.657614, 5327 },
    { 55.587708, 37.657689, 5343 },
    { 55.587562, 37.657700, 5359 },
    { 55.587538, 37.657753, 5363 },
    { 55.587477, 37.657775, 5370 },
    { 55.587392, 37.657818, 5380 },
    { 55.587313, 37.657903, 5390 },
    { 55.587410, 37.657775, 5404 },
    { 55.587671, 37.657571, 5435 },
    { 55.587677, 37.657732, 5445 },
    { 55.587623, 37.657753, 5452 },
    { 55.587544, 37.657818, 5461 },
    { 55.587447, 37.657839, 5472 },
    { 55.587380, 37.657882, 5480 },
    { 55.587265, 37.658011, 5495 },
    { 55.587144, 37.657903, 5510 },
    { 55.587083, 37.658043, 5521 },
    { 55.587004, 37.658129, 5532 },
    { 55.586956, 37.658139, 5537 },
    { 55.586853, 37.658257, 5551 },
    { 55.586780, 37.658290, 5559 },
    { 55.586640, 37.658279, 5575 },
    { 55.586549, 37.658354, 5586 },
    { 55.586446, 37.658408, 5598 },
    { 55.586368, 37.658504, 5608 },
    { 55.586295, 37.658569, 5617 },
    { 55.586246, 37.658622, 5624 },
    { 55.586137, 37.658762, 5639 },
    { 55.585979, 37.658880, 5658 },
    { 55.585895, 37.658858, 5667 },
    { 55.585779, 37.658912, 5681 },
    { 55.585688, 37.659019, 5693 },
    { 55.585567, 37.659084, 5707 },
    { 55.585494, 37.659105, 5715 },
    { 55.585440, 37.659180, 5723 },
    { 55.585337, 37.659212, 5734 },
    { 55.585215, 37.659255, 5748 },
    { 55.585100, 37.659298, 5761 },
    { 55.585064, 37.659363, 5767 },
    { 55.584894, 37.659459, 5787 },
    { 55.584840, 37.659448, 5793 },
    { 55.584761, 37.659481, 5802 },
    { 55.584645, 37.659534, 5815 },
    { 55.584542, 37.659566, 5827 },
    { 55.584427, 37.659566, 5840 },
    { 55.584373, 37.659545, 5846 },
    { 55.584209, 37.659534, 5864 },
    { 55.584094, 37.659556, 5877 },
    { 55.584003, 37.659577, 5887 },
    { 55.583863, 37.659609, 5903 },
    { 55.583790, 37.659663, 5912 },
    { 55.583675, 37.659684, 5924 },
    { 55.583590, 37.659695, 5934 },
    { 55.583463, 37.659738, 5948 },
    { 55.583360, 37.659824, 5961 },
    { 55.583239, 37.659824, 5974 },
    { 55.583129, 37.659899, 5988 },
    { 55.583069, 37.659974, 5996 },
    { 55.582899, 37.660060, 6015 },
    { 55.582899, 37.660017, 6018 },
    { 55.582857, 37.660060, 6023 },
    { 55.582747, 37.660124, 6036 },
    { 55.582656, 37.660081, 6047 },
    { 55.582008, 37.660146, 6119 },
    { 55.581207, 37.660103, 6208 },
    { 55.580722, 37.660060, 6262 },
    { 55.580067, 37.660360, 6337 },
    { 55.579436, 37.660446, 6408 }
};

void getParamsFromConsole ()
{
    QString str;
    QTextStream in(stdin);
    QTextStream out(stdout);

    while (true)
    {
        str = in.readLine();
        QStringList cmd = str.split(" ");

        if (cmd.at(0) == "z")
        {
            static int i = 0;
            elMap->setMetrometer (coords[i][2]);
            elMap->checkMap (coords[i][0], coords[i][1]);
            i ++;
        }
        else if (cmd.at(0) == "s")
        {
            systemState->setSpeed( cmd.at(1).toInt() );
            out << "Speed: " << systemState->getSpeed() << endl;
        }
        else if (cmd.at(0) == "r")
        {
            systemState->setSpeedRestriction( cmd.at(1).toInt() );
            out << "Speed restriction: " << systemState->getSpeedRestriction() << endl;
        }
        else if (cmd.at(0) == "g")
        {
            if (cmd.size() == 3)
            {
                systemState->setLongitude( cmd.at(1).toDouble() );
                systemState->setLatitude( cmd.at(2).toDouble() );
            }
            out << "Longitude: " << systemState->getLongitude()
                << "; Lattitude: " << systemState->getLatitude() << endl;
        }
        else if (cmd.at(0) == "gg")
        {
            if (cmd.size() == 3)
            {
                systemState->setLongitude(systemState->getLongitude() + cmd.at(1).toDouble() );
                systemState->setLatitude(systemState->getLatitude() + cmd.at(2).toDouble() );
            }
            out << "Longitude: " << systemState->getLongitude()
                << "; Lattitude: " << systemState->getLatitude() << endl;
        }
        else if (cmd.at(0) == "c")
        {
            systemState->setLight( cmd.at(1).toInt() );
            out << "Liht: " << systemState->getLight() << endl;
        }
        else if (cmd.at(0) == "a")
        {
            systemState->setAlsnFreqTarget( cmd.at(1).toInt() );
            out << "AlsnFreqTarget: " << systemState->getAlsnFreqTarget() << endl;
        }
        else if (cmd.at(0) == "epv")
        {
            systemState->setIsEpvReady( cmd.at(1) == "1" );
            out << "IsEpvReady: " << systemState->getIsEpvReady() << endl;
        }
        else if (cmd.at(0) == "b")
        {
            systemState->setIsEpvReleased( cmd.at(1) == "1" );
            out << "IsEpvReady: " << systemState->getIsEpvReleased() << endl;
        }
        else if (cmd.at(0) == "dm")
        {
            systemState->setDriveModeFact( cmd.at(1).toInt() );
            out << "DriveModeFact: " << systemState->getDriveModeFact() << endl;
        }
        else if (cmd.at(0) == "tdm")
        {
            systemState->setDriveModeTarget( cmd.at(1).toInt() );
            out << "DriveModeTarget: " << systemState->getDriveModeTarget() << endl;
        }
        else if (cmd.at(0) == "iw")
        {
            systemState->setIronWheels( cmd.at(1) == "1" );
            out << "Iron Wheels: " << systemState->getIronWheels() << endl;
        }
        // Тяга
        else if (cmd.at(0) == "tr")
        {
            systemState->setIsTractionOn( cmd.at(1) == "1" );
            out << "Traction is: " << systemState->getIsTractionOn() << endl;
        }
        // Направление движения
        else if (cmd.at(0) == "dir")
        {
            systemState->setDirection( cmd.at(1).toInt() );
            out << "Now Direction is: " << systemState->getDirection() << endl;
        }
        else if (cmd.at(0) == "wt")
        {
            systemState->setWarningText( cmd.at(1) );
            out << "Now Warning Text is: " << systemState->getWarningText() << endl;
        }
        else if (cmd.at(0) == "it")
        {
            systemState->setInfoText( cmd.at(1) );
            out << "Now Info Text is: " << systemState->getInfoText() << endl;
        }
        else
        {
            out << "! unknown command. Try this:" << endl;
            out << "tdm {0/1/2/3/4} Целевой режим движения: П/М/Р/Д/Т" << endl;
            out << "dm {0/1/2/3/4} Фактический режим движения: П/М/Р/Д/Т" << endl;
            out << "epb {1/0} Ключ ЭПК: вкл/выкл" << endl;
            out << "b {1/0} Экстренное торможение: вкл/выкл" << endl;
            out << "iw {1/0} IronWheels" << endl;
            out << "tr {1/0} Тяга: вкл/выкл" << endl;
            out << "dir {1/-1/0} Направление движения: вперёд/назад/стоим" << endl;
            out << "wt {text} Текст предупреждения" << endl;
            out << "it {text} Текст совета" << endl;
        }
    }
}




Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);

    qmlRegisterType<SystemStateViewModel>("views", 1, 0, "SystemStateView");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/QtMon/main.qml"));
#ifdef ON_DEVICE
    viewer.showFullScreen();
#else
    viewer.showExpanded();
#endif

    QObject *object = viewer.rootObject();
    systemState = object->findChild<SystemStateViewModel*>("stateView");
    elMap = new Navigation::ElectroincMap();

#ifdef WITH_CAN
    //QtConcurrent::run(getParamsFromCan);
    //Здесь подключаюсь я.
    iodriver = new iodrv(systemState);
    speedAgregator = new SpeedAgregator();
    emapCanEmitter = new EMapCanEmitter();

    // Создание и подключение «обработчиков»
    // -> Отбработчик нажатия РМП <-
    rmp_key_hdlr = new rmp_key_handler();

    QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonPressed()), rmp_key_hdlr, SLOT(rmp_key_pressed()));
    QObject::connect(iodriver, SIGNAL(signal_ssps_mode(int)), rmp_key_hdlr, SLOT(ssps_mode_received(int)));
    QObject::connect(iodriver, SIGNAL(signal_driving_mode(int)), rmp_key_hdlr, SLOT(driving_mode_received(int)));

    QObject::connect(rmp_key_hdlr, SIGNAL(target_driving_mode_changed(int)), systemState, SLOT(setDriveModeTarget(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(actual_driving_mode_changed(int)), systemState, SLOT(setDriveModeFact(int)));
    QObject::connect(rmp_key_hdlr, SIGNAL(rmp_key_pressed_send()), iodriver, SLOT(slot_rmp_key_down()));
    // <- Отбработчик нажатия РМП ->

    // Переносить ли эти события из iodrv в обработчики
    QObject::connect(iodriver, SIGNAL(signal_iron_wheels(bool)), systemState, SLOT(setIronWheels(bool)));



    // Для отладки
    QObject::connect(iodriver, SIGNAL(signal_speed_earth(double)), systemState, SLOT(setSpeedFromEarth(double)));
    QObject::connect(iodriver, SIGNAL(signal_speed_sky(double)), systemState, SLOT(setSpeedFromSky(double)));

    //Скорость и ограничения
    QObject::connect(iodriver, SIGNAL(signal_speed_earth(double)), speedAgregator, SLOT(getSpeedFromEarth(double)));
    QObject::connect(iodriver, SIGNAL(signal_speed_sky(double)), speedAgregator, SLOT(getSpeedFromSky(double)));
    QObject::connect(iodriver, SIGNAL(signal_is_on_road(bool)), speedAgregator, SLOT(getIsOnRoad(bool)));
    QObject::connect(speedAgregator, SIGNAL(speedChanged(double)), systemState, SLOT(setSpeed(double)));
    QObject::connect(speedAgregator, SIGNAL(speedIsValidChanged(bool)), systemState, SLOT(setSpeedIsValid(bool)));
    QObject::connect(iodriver, SIGNAL(signal_speed_limit(int)), systemState, SLOT(setSpeedRestriction(int)));
    QObject::connect(iodriver, SIGNAL(signal_target_speed(int)), systemState, SLOT(setTargetSpeed(int)));
    QObject::connect(iodriver, SIGNAL(signal_acceleration(double)), systemState, SLOT(setAcceleration(double)));
    //Состояние системы
    QObject::connect(iodriver, SIGNAL(signal_epv_key(bool)), systemState, SLOT(setIsEpvReady(bool)));
    QObject::connect(iodriver, SIGNAL(signal_epv_released(bool)), systemState, SLOT(setIsEpvReleased(bool)));
    //Одометр
    QObject::connect(iodriver, SIGNAL(signal_passed_distance(int)), systemState, SLOT(setMilage(int)));
    //Светофоры
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_light(int)), systemState, SLOT(setLight(int)));
    QObject::connect(iodriver, SIGNAL(signal_trafficlight_freq(int)), systemState, SLOT(setAlsnFreqFact(int)));

    //QObject::connect(iodriver, SIGNAL(signal_driving_mode(int)), systemState, SLOT(setDriveModeFact(int)));
    QObject::connect(iodriver, SIGNAL(signal_vigilance(bool)), systemState, SLOT(setIsVigilanceRequired(bool)));
    QObject::connect(iodriver, SIGNAL(signal_movement_direction(int)), systemState, SLOT(setDirection(int)));
    QObject::connect(iodriver, SIGNAL(signal_reg_tape_avl(bool)), systemState, SLOT(setIsRegistrationTapeActive(bool)));

    QObject::connect(iodriver, SIGNAL(signal_autolock_type(int)), systemState, SLOT(setAutolockTypeFact(int)));
    QObject::connect(systemState, SIGNAL(AutolockTypeTargetChanged()), iodriver, SLOT(slot_autolock_type_target_changed()));

    QObject::connect(iodriver, SIGNAL(signal_pressure_tc(QString)), systemState, SLOT(setPressureTC(QString)));
    QObject::connect(iodriver, SIGNAL(signal_pressure_tm(QString)), systemState, SLOT(setPressureTM(QString)));

    QObject::connect(iodriver, SIGNAL(signal_lat(double)), systemState, SLOT(setLatitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_lon(double)), systemState, SLOT(setLongitude(double)));
    QObject::connect(iodriver, SIGNAL(signal_time(QString)), systemState, SLOT(setTime(QString)));
    QObject::connect(iodriver, SIGNAL(signal_date(QString)), systemState, SLOT(setDate(QString)));

    QObject::connect(iodriver, SIGNAL(signal_traction(bool)), systemState, SLOT(setIsTractionOn(bool)));

    //QObject::connect(systemState, SIGNAL(AlsnFreqTargetChanged()), iodriver, SLOT(slot_f_key_down()));
    QObject::connect(systemState, SIGNAL(DisableRedButtonPressed()), iodriver, SLOT(slot_vk_key_down()));
    QObject::connect(systemState, SIGNAL(DisableRedButtonReleased()), iodriver, SLOT(slot_vk_key_up()));
    //QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonPressed()), iodriver, SLOT(slot_rmp_key_down()));
    //QObject::connect(systemState, SIGNAL(ChangeDrivemodeButtonReleased()), iodriver, SLOT(slot_rmp_key_up()));

    // TODO: QObject::connect(systemState, SIGNAL(), iodriver, SLOT(slot_vk_key_up()));
    // TODO: QObject::connect(systemState, SIGNAL(), iodriver, SLOT(slot_rmp_key_up()));

    // Электронная карта
    QObject::connect (elMap, SIGNAL(onUpcomingTargets(std::vector<EMapTarget>)), emapCanEmitter, SLOT(setObjectsList(std::vector<EMapTarget>)));
    QObject::connect (emapCanEmitter, SIGNAL(sendNextObjectToCan(can_frame)), iodriver, SLOT(slot_write_can0_message(can_frame)));

    iodriver->start(argv[1], argv[2], (QString(argv[3]).toInt() == 0) ? gps : can);

#else
    QtConcurrent::run(getParamsFromConsole);
#endif


    printf("\033[0;36;40m HELLO!!!\033[0;37;40m");

    elMap->load ("./map.gps");
    elMap->setTrackNumber(2);
    for (int i = 0; i < 400; i++)
    {
        elMap->setMetrometer (coords[i][2]);
        elMap->checkMap (coords[i][0], coords[i][1]);
        //getc(stdin);
    }

    return app->exec();
}
