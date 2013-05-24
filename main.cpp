#include <QApplication>
#include "qmlapplicationviewer.h"
#include "systemstateviewmodel.h"
#include "qtconcurrentrun.h"
#include <QTextStream>
#include <QTextCodec>
#include "electroincmap.h"
#include "math.h"

#ifdef WITH_CAN
#include "iodrv/iodrv.h"
#endif

#include <iostream>

SystemStateViewModel *systemState ;

#ifdef WITH_CAN
iodrv* iodriver;
SpeedAgregator* speedAgregator;
rmp_key_handler* rmp_key_hdlr;
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

Navigation::ElectroincMap elMap;

double coords[][3] = {
    { 55.660824, 37.716284, 0 },
    { 55.660576, 37.715700, 46 },
    { 55.660255, 37.715410, 86 },
    { 55.660010, 37.714825, 132 },
    { 55.659719, 37.714632, 166 },
    { 55.659492, 37.714396, 195 },
    { 55.659365, 37.714080, 220 },
    { 55.659078, 37.713865, 254 },
    { 55.658884, 37.713650, 280 },
    { 55.658718, 37.713538, 300 },
    { 55.658636, 37.713313, 316 },
    { 55.658342, 37.712878, 359 },
    { 55.658128, 37.712706, 385 },
    { 55.657864, 37.712476, 418 },
    { 55.657855, 37.712256, 432 },
    { 55.657604, 37.712041, 463 },
    { 55.657386, 37.711848, 490 },
    { 55.657117, 37.711220, 539 },
    { 55.656608, 37.711070, 597 },
    { 55.656415, 37.710582, 634 },
    { 55.656167, 37.710174, 672 },
    { 55.655791, 37.709692, 723 },
    { 55.655461, 37.709321, 767 },
    { 55.655001, 37.708737, 830 },
    { 55.654638, 37.708152, 884 },
    { 55.654399, 37.707948, 914 },
    { 55.654124, 37.707578, 952 },
    { 55.653942, 37.707170, 985 },
    { 55.653482, 37.706687, 1044 },
    { 55.653146, 37.706146, 1095 },
    { 55.652931, 37.705706, 1131 },
    { 55.652668, 37.705395, 1166 },
    { 55.652483, 37.705083, 1195 },
    { 55.652184, 37.704649, 1238 },
    { 55.651866, 37.704220, 1282 },
    { 55.651536, 37.703823, 1326 },
    { 55.651303, 37.703469, 1361 },
    { 55.650773, 37.703088, 1424 },
    { 55.650489, 37.702337, 1481 },
    { 55.650056, 37.701763, 1541 },
    { 55.649632, 37.701403, 1593 },
    { 55.649251, 37.700593, 1659 },
    { 55.648688, 37.700046, 1731 },
    { 55.648630, 37.699397, 1772 },
    { 55.648340, 37.699091, 1810 },
    { 55.648261, 37.698576, 1843 },
    { 55.648094, 37.698416, 1864 },
    { 55.648028, 37.698099, 1885 },
    { 55.647904, 37.698019, 1900 },
    { 55.647786, 37.697740, 1922 },
    { 55.647795, 37.697471, 1939 },
    { 55.647634, 37.697402, 1957 },
    { 55.647513, 37.697091, 1981 },
    { 55.647310, 37.696543, 2022 },
    { 55.647086, 37.696404, 2048 },
    { 55.646996, 37.696055, 2073 },
    { 55.646847, 37.695867, 2093 },
    { 55.646696, 37.695551, 2119 },
    { 55.646520, 37.695363, 2142 },
    { 55.646432, 37.695111, 2160 },
    { 55.646202, 37.694929, 2188 },
    { 55.645945, 37.694639, 2222 },
    { 55.645721, 37.694317, 2254 },
    { 55.645312, 37.694038, 2303 },
    { 55.645010, 37.693609, 2346 },
    { 55.644631, 37.693185, 2396 },
    { 55.643832, 37.692445, 2496 },
    { 55.643681, 37.692118, 2523 },
    { 55.643381, 37.691662, 2567 },
    { 55.643205, 37.691662, 2586 },
    { 55.642936, 37.691372, 2621 },
    { 55.642763, 37.691050, 2649 },
    { 55.642600, 37.690975, 2668 },
    { 55.642406, 37.690782, 2693 },
    { 55.642382, 37.690589, 2705 },
    { 55.642234, 37.690648, 2722 },
    { 55.642070, 37.690390, 2746 },
    { 55.641801, 37.690187, 2779 },
    { 55.641534, 37.689886, 2814 },
    { 55.641225, 37.689811, 2849 },
    { 55.641029, 37.689430, 2881 },
    { 55.640738, 37.689441, 2913 },
    { 55.640681, 37.689237, 2928 },
    { 55.640547, 37.689162, 2943 },
    { 55.640478, 37.689065, 2953 },
    { 55.640429, 37.689065, 2958 },
    { 55.640399, 37.688969, 2965 },
    { 55.640332, 37.689001, 2973 },
    { 55.640287, 37.688899, 2981 },
    { 55.640229, 37.688904, 2988 },
    { 55.640148, 37.688770, 3000 },
    { 55.640099, 37.688760, 3005 },
    { 55.640008, 37.688722, 3016 },
    { 55.639954, 37.688599, 3026 },
    { 55.639848, 37.688749, 3041 },
    { 55.639872, 37.688572, 3052 },
    { 55.639803, 37.688615, 3060 },
    { 55.639754, 37.688518, 3068 },
    { 55.639660, 37.688347, 3083 },
    { 55.639612, 37.688427, 3091 },
    { 55.639500, 37.688266, 3107 },
    { 55.639433, 37.688266, 3114 },
    { 55.639376, 37.688169, 3123 },
    { 55.639279, 37.688196, 3134 },
    { 55.639209, 37.688089, 3144 },
    { 55.639055, 37.688116, 3161 },
    { 55.639100, 37.687966, 3172 },
    { 55.638958, 37.687998, 3188 },
    { 55.638867, 37.687880, 3201 },
    { 55.638731, 37.687864, 3216 },
    { 55.638670, 37.687773, 3225 },
    { 55.638573, 37.687842, 3236 },
    { 55.638416, 37.687708, 3256 },
    { 55.638261, 37.687574, 3275 },
    { 55.638062, 37.687660, 3297 },
    { 55.637956, 37.687553, 3311 },
    { 55.637898, 37.687467, 3319 },
    { 55.637735, 37.687445, 3338 },
    { 55.637641, 37.687424, 3348 },
    { 55.637595, 37.687333, 3356 },
    { 55.637456, 37.687381, 3372 },
    { 55.637417, 37.687290, 3379 },
    { 55.637196, 37.687284, 3403 },
    { 55.637078, 37.687097, 3421 },
    { 55.636987, 37.687134, 3431 },
    { 55.636829, 37.686973, 3452 },
    { 55.636109, 37.686791, 3532 },
    { 55.635379, 37.686410, 3617 },
    { 55.635006, 37.686512, 3659 },
    { 55.634555, 37.686061, 3717 },
    { 55.634122, 37.686238, 3766 },
    { 55.633880, 37.685906, 3800 },
    { 55.633480, 37.685975, 3845 },
    { 55.633138, 37.685659, 3888 },
    { 55.633150, 37.685852, 3900 },
    { 55.633389, 37.685755, 3927 },
    { 55.632829, 37.685530, 3991 },
    { 55.632690, 37.685616, 4007 },
    { 55.632396, 37.685455, 4042 },
    { 55.632330, 37.685348, 4052 },
    { 55.631996, 37.685444, 4089 },
    { 55.631818, 37.685117, 4118 },
    { 55.631415, 37.685021, 4163 },
    { 55.631497, 37.685300, 4183 },
    { 55.631239, 37.684956, 4219 },
    { 55.630782, 37.684645, 4273 },
    { 55.630564, 37.684339, 4304 },
    { 55.630213, 37.684114, 4345 },
    { 55.629977, 37.683776, 4379 },
    { 55.629383, 37.683202, 4454 },
    { 55.629262, 37.682896, 4478 },
    { 55.628841, 37.682719, 4526 },
    { 55.628611, 37.682038, 4576 },
    { 55.628265, 37.681866, 4616 },
    { 55.628026, 37.681373, 4657 },
    { 55.627605, 37.681105, 4706 },
    { 55.627330, 37.680525, 4754 },
    { 55.627003, 37.680214, 4795 },
    { 55.626766, 37.679935, 4827 },
    { 55.626312, 37.679184, 4896 },
    { 55.625967, 37.679066, 4935 },
    { 55.625585, 37.678294, 4999 },
    { 55.625113, 37.677934, 5056 },
    { 55.624561, 37.677087, 5138 },
    { 55.624113, 37.676722, 5192 },
    { 55.623598, 37.675821, 5273 },
    { 55.622962, 37.675381, 5349 },
    { 55.622375, 37.674361, 5440 },
    { 55.621605, 37.673879, 5531 },
    { 55.621211, 37.672698, 5617 },
    { 55.620430, 37.672302, 5708 },
    { 55.620194, 37.671679, 5755 },
    { 55.619715, 37.671175, 5817 },
    { 55.619309, 37.670563, 5876 },
    { 55.619000, 37.670145, 5919 },
    { 55.618588, 37.669630, 5975 },
    { 55.618200, 37.668879, 6039 },
    { 55.617625, 37.667677, 6138 },
    { 55.617279, 37.666959, 6197 },
    { 55.616922, 37.666079, 6265 },
    { 55.616565, 37.664995, 6344 },
    { 55.616256, 37.663815, 6426 },
    { 55.616007, 37.663064, 6481 },
    { 55.615710, 37.661830, 6565 },
    { 55.615565, 37.661272, 6603 },
    { 55.615486, 37.660929, 6627 },
    { 55.615450, 37.660618, 6647 },
    { 55.615347, 37.660414, 6664 },
    { 55.615328, 37.660178, 6679 },
    { 55.615280, 37.659867, 6699 },
    { 55.615171, 37.659577, 6721 },
    { 55.615080, 37.659255, 6743 },
    { 55.615001, 37.659019, 6761 },
    { 55.614874, 37.658719, 6784 },
    { 55.614765, 37.658515, 6802 },
    { 55.614710, 37.658279, 6818 },
    { 55.614632, 37.658097, 6832 },
    { 55.614547, 37.657828, 6852 },
    { 55.614492, 37.657603, 6867 },
    { 55.614389, 37.657528, 6879 },
    { 55.614323, 37.657206, 6901 },
    { 55.614262, 37.656906, 6921 },
    { 55.614129, 37.656648, 6943 },
    { 55.614044, 37.656412, 6960 },
    { 55.613953, 37.656208, 6977 },
    { 55.613905, 37.655983, 6992 },
    { 55.613820, 37.655801, 7007 },
    { 55.613753, 37.655640, 7019 },
    { 55.613668, 37.655457, 7034 },
    { 55.613547, 37.655243, 7053 },
    { 55.613486, 37.654985, 7071 },
    { 55.613371, 37.654760, 7090 },
    { 55.613305, 37.654717, 7098 },
    { 55.613208, 37.654513, 7114 },
    { 55.613129, 37.654320, 7129 },
    { 55.612990, 37.654138, 7149 },
    { 55.612868, 37.653923, 7168 },
    { 55.612808, 37.653719, 7182 },
    { 55.612681, 37.653537, 7200 },
    { 55.612602, 37.653387, 7213 },
    { 55.612487, 37.653172, 7232 },
    { 55.612390, 37.653022, 7246 },
    { 55.612262, 37.652850, 7264 },
    { 55.612159, 37.652668, 7280 },
    { 55.612020, 37.652464, 7300 },
    { 55.611953, 37.652292, 7313 },
    { 55.611838, 37.652131, 7330 },
    { 55.611735, 37.652067, 7342 },
    { 55.611632, 37.651906, 7357 },
    { 55.611578, 37.651767, 7368 },
    { 55.611481, 37.651724, 7379 },
    { 55.611384, 37.651509, 7396 },
    { 55.611311, 37.651488, 7404 },
    { 55.611208, 37.651348, 7419 },
    { 55.611117, 37.651252, 7430 },
    { 55.610996, 37.651048, 7449 },
    { 55.610784, 37.650865, 7475 },
    { 55.610608, 37.650704, 7497 },
    { 55.610469, 37.650608, 7514 },
    { 55.610396, 37.650383, 7530 },
    { 55.610232, 37.650393, 7548 },
    { 55.610105, 37.650275, 7564 },
    { 55.610033, 37.650093, 7578 },
    { 55.609893, 37.650071, 7594 },
    { 55.609711, 37.649932, 7616 },
    { 55.609590, 37.649846, 7631 },
    { 55.609445, 37.649760, 7648 },
    { 55.609293, 37.649535, 7670 },
    { 55.609154, 37.649556, 7685 },
    { 55.609027, 37.649320, 7706 },
    { 55.608899, 37.649310, 7720 },
    { 55.608687, 37.649353, 7744 },
    { 55.608481, 37.649052, 7773 },
    { 55.608366, 37.648966, 7787 },
    { 55.608245, 37.649020, 7801 },
    { 55.608105, 37.648891, 7819 },
    { 55.608027, 37.648988, 7829 },
    { 55.607875, 37.648838, 7848 },
    { 55.607730, 37.648795, 7865 },
    { 55.607687, 37.648795, 7870 },
    { 55.607536, 37.648762, 7887 },
    { 55.607475, 37.648720, 7894 },
    { 55.607390, 37.648687, 7903 },
    { 55.607305, 37.648644, 7913 },
    { 55.607172, 37.648580, 7929 },
    { 55.607099, 37.648602, 7937 },
    { 55.606978, 37.648569, 7950 },
    { 55.606875, 37.648548, 7962 },
    { 55.606766, 37.648526, 7974 },
    { 55.606596, 37.648505, 7993 },
    { 55.606475, 37.648494, 8007 },
    { 55.606366, 37.648484, 8019 },
    { 55.606257, 37.648430, 8031 },
    { 55.606136, 37.648473, 8045 },
    { 55.606045, 37.648473, 8055 },
    { 55.605942, 37.648430, 8067 },
    { 55.605730, 37.648419, 8091 },
    { 55.605627, 37.648462, 8102 },
    { 55.605530, 37.648462, 8113 },
    { 55.605433, 37.648484, 8124 },
    { 55.605354, 37.648494, 8133 },
    { 55.605209, 37.648494, 8149 },
    { 55.604930, 37.648559, 8180 },
    { 55.604875, 37.648569, 8186 },
    { 55.604815, 37.648612, 8194 },
    { 55.604724, 37.648537, 8205 },
    { 55.604609, 37.648569, 8218 },
    { 55.604548, 37.648644, 8226 },
    { 55.604518, 37.648612, 8230 },
    { 55.604421, 37.648591, 8241 },
    { 55.604348, 37.648623, 8249 },
    { 55.604263, 37.648666, 8259 },
    { 55.604184, 37.648698, 8268 },
    { 55.604105, 37.648720, 8277 },
    { 55.604009, 37.648741, 8288 },
    { 55.603930, 37.648773, 8297 },
    { 55.603827, 37.648816, 8308 },
    { 55.603742, 37.648848, 8318 },
    { 55.603566, 37.648966, 8339 },
    { 55.603275, 37.648934, 8371 },
    { 55.603221, 37.648977, 8378 },
    { 55.603069, 37.649031, 8395 },
    { 55.602948, 37.649106, 8409 },
    { 55.602796, 37.649181, 8427 },
    { 55.602705, 37.649277, 8439 },
    { 55.602590, 37.649267, 8452 },
    { 55.602499, 37.649406, 8465 },
    { 55.602372, 37.649513, 8481 },
    { 55.602233, 37.649556, 8496 },
    { 55.602093, 37.649750, 8516 },
    { 55.602002, 37.649760, 8526 },
    { 55.601808, 37.649953, 8551 },
    { 55.601711, 37.649953, 8562 },
    { 55.601602, 37.650114, 8577 },
    { 55.601499, 37.650146, 8589 },
    { 55.601414, 37.650211, 8599 },
    { 55.601348, 37.650329, 8610 },
    { 55.601275, 37.650340, 8618 },
    { 55.601117, 37.650404, 8636 },
    { 55.600978, 37.650629, 8657 },
    { 55.600869, 37.650651, 8669 },
    { 55.600748, 37.650758, 8684 },
    { 55.600590, 37.650812, 8702 },
    { 55.600505, 37.650983, 8716 },
    { 55.600402, 37.650983, 8728 },
    { 55.600323, 37.651101, 8739 },
    { 55.600160, 37.651262, 8760 },
    { 55.600069, 37.651327, 8771 },
    { 55.599954, 37.651412, 8785 },
    { 55.599893, 37.651477, 8793 },
    { 55.599760, 37.651616, 8810 },
    { 55.599681, 37.651713, 8821 },
    { 55.599572, 37.651745, 8833 },
    { 55.599511, 37.651809, 8841 },
    { 55.599457, 37.651842, 8847 },
    { 55.599372, 37.651895, 8857 },
    { 55.599299, 37.651970, 8867 },
    { 55.599196, 37.652024, 8878 },
    { 55.599111, 37.652110, 8889 },
    { 55.599032, 37.652185, 8899 },
    { 55.598917, 37.652260, 8913 },
    { 55.598735, 37.652432, 8936 },
    { 55.598644, 37.652518, 8947 },
    { 55.598572, 37.652550, 8956 },
    { 55.598462, 37.652614, 8968 },
    { 55.598396, 37.652603, 8976 },
    { 55.598287, 37.652646, 8988 },
    { 55.598178, 37.652743, 9002 },
    { 55.597965, 37.653000, 9031 },
    { 55.597850, 37.652968, 9043 },
    { 55.597711, 37.653033, 9059 },
    { 55.597656, 37.653011, 9066 },
    { 55.597541, 37.653097, 9080 },
    { 55.597444, 37.653226, 9093 },
    { 55.597329, 37.653290, 9106 },
    { 55.597189, 37.653354, 9123 },
    { 55.597099, 37.653333, 9133 },
    { 55.597026, 37.653419, 9142 },
    { 55.596947, 37.653505, 9153 },
    { 55.596899, 37.653548, 9159 },
    { 55.596795, 37.653558, 9170 },
    { 55.596765, 37.653558, 9174 },
    { 55.596698, 37.653612, 9182 },
    { 55.596650, 37.653644, 9188 },
    { 55.596559, 37.653687, 9198 },
    { 55.596450, 37.653708, 9210 },
    { 55.596298, 37.653848, 9229 },
    { 55.596220, 37.653923, 9239 },
    { 55.596123, 37.653977, 9250 },
    { 55.596020, 37.653977, 9262 },
    { 55.595898, 37.654009, 9276 },
    { 55.595638, 37.654138, 9306 },
    { 55.595571, 37.654234, 9315 },
    { 55.595547, 37.654277, 9319 },
    { 55.595474, 37.654299, 9327 },
    { 55.595359, 37.654352, 9340 },
    { 55.595231, 37.654341, 9355 },
    { 55.595165, 37.654438, 9364 },
    { 55.595086, 37.654470, 9373 },
    { 55.595025, 37.654535, 9381 },
    { 55.594880, 37.654599, 9398 },
    { 55.594789, 37.654717, 9410 },
    { 55.594571, 37.654728, 9435 },
    { 55.594468, 37.654728, 9446 },
    { 55.594377, 37.654738, 9456 },
    { 55.594274, 37.654792, 9468 },
    { 55.594086, 37.654856, 9489 },
    { 55.594043, 37.654856, 9494 },
    { 55.593934, 37.654846, 9506 },
    { 55.593880, 37.654889, 9513 },
    { 55.593813, 37.654889, 9520 },
    { 55.593704, 37.654846, 9533 },
    { 55.593625, 37.654814, 9542 },
    { 55.593510, 37.654846, 9555 },
    { 55.593389, 37.654824, 9568 },
    { 55.593219, 37.654781, 9587 },
    { 55.593007, 37.654577, 9614 },
    { 55.592849, 37.654620, 9632 },
    { 55.592728, 37.654781, 9649 },
    { 55.592673, 37.654749, 9655 },
    { 55.592346, 37.654803, 9692 },
    { 55.592231, 37.654835, 9705 },
    { 55.592006, 37.654932, 9730 },
    { 55.591739, 37.655050, 9761 },
    { 55.591455, 37.655200, 9794 },
    { 55.591345, 37.655232, 9806 },
    { 55.591164, 37.655371, 9828 },
    { 55.591042, 37.655457, 9843 },
    { 55.590854, 37.655565, 9865 },
    { 55.590727, 37.655586, 9879 },
    { 55.590588, 37.655672, 9895 },
    { 55.590406, 37.655779, 9917 },
    { 55.590242, 37.655854, 9936 },
    { 55.590139, 37.655951, 9949 },
    { 55.590036, 37.655951, 9960 },
    { 55.589951, 37.656004, 9970 },
    { 55.589890, 37.655994, 9977 },
    { 55.589787, 37.656122, 9991 },
    { 55.589684, 37.656133, 10002 },
    { 55.589569, 37.656187, 10016 },
    { 55.589454, 37.656230, 10029 },
    { 55.589260, 37.656337, 10051 },
    { 55.589151, 37.656316, 10063 },
    { 55.589048, 37.656380, 10076 },
    { 55.588963, 37.656509, 10088 },
    { 55.588890, 37.656595, 10098 },
    { 55.588787, 37.656605, 10109 },
    { 55.588708, 37.656637, 10118 },
    { 55.588635, 37.656659, 10126 },
    { 55.588532, 37.656648, 10138 },
    { 55.588465, 37.656745, 10148 },
    { 55.588393, 37.656788, 10156 },
    { 55.588308, 37.656841, 10166 },
    { 55.588199, 37.656959, 10180 },
    { 55.588065, 37.657024, 10196 },
    { 55.587956, 37.656970, 10208 },
    { 55.587799, 37.657002, 10226 },
    { 55.587611, 37.657238, 10252 },
    { 55.587477, 37.657378, 10269 },
    { 55.587356, 37.657335, 10283 },
    { 55.587229, 37.657356, 10297 },
    { 55.587138, 37.657442, 10308 },
    { 55.587089, 37.657485, 10314 },
    { 55.586992, 37.657582, 10327 },
    { 55.586883, 37.657646, 10339 },
    { 55.586792, 37.657689, 10350 },
    { 55.586683, 37.657764, 10363 },
    { 55.587835, 37.657453, 10492 },
    { 55.586501, 37.657807, 10642 },
    { 55.586319, 37.657914, 10664 },
    { 55.586234, 37.657979, 10674 },
    { 55.586089, 37.658032, 10691 },
    { 55.585895, 37.658182, 10714 },
    { 55.585773, 37.658225, 10728 },
    { 55.585597, 37.658311, 10748 },
    { 55.585482, 37.658343, 10761 },
    { 55.585325, 37.658397, 10779 },
    { 55.585015, 37.658558, 10815 },
    { 55.584712, 37.658869, 10854 },
    { 55.584639, 37.658740, 10865 },
    { 55.584621, 37.658944, 10878 },
    { 55.584518, 37.658730, 10896 },
    { 55.584809, 37.658783, 10928 },
    { 55.584645, 37.658858, 10947 },
    { 55.584457, 37.659041, 10971 },
    { 55.584658, 37.658676, 11003 },
    { 55.584482, 37.658837, 11025 },
    { 55.584833, 37.658794, 11064 },
    { 55.584676, 37.658579, 11086 },
    { 55.584530, 37.659030, 11119 },
    { 55.584457, 37.658837, 11134 },
    { 55.584421, 37.658955, 11142 },
    { 55.584421, 37.658848, 11149 },
    { 55.584360, 37.658998, 11160 },
    { 55.584373, 37.658837, 11171 },
    { 55.584288, 37.659041, 11187 },
    { 55.584251, 37.658944, 11194 },
    { 55.584555, 37.658762, 11230 },
    { 55.584342, 37.659009, 11258 },
    { 55.584288, 37.658944, 11265 },
    { 55.584124, 37.659169, 11288 },
    { 55.584009, 37.659148, 11301 },
    { 55.583875, 37.659234, 11317 },
    { 55.583742, 37.659330, 11333 },
    { 55.583572, 37.659448, 11353 },
    { 55.583433, 37.659513, 11369 },
    { 55.583269, 37.659566, 11388 },
    { 55.583154, 37.659620, 11401 },
    { 55.583014, 37.659695, 11417 },
    { 55.582814, 37.659738, 11440 },
    { 55.582711, 37.659781, 11451 },
    { 55.582523, 37.659824, 11473 },
    { 55.582402, 37.659888, 11487 },
    { 55.582274, 37.659878, 11501 },
    { 55.582147, 37.659942, 11516 },
    { 55.581947, 37.659974, 11538 }
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
            elMap.setMetrometer (coords[i][2]);
            elMap.checkMap (coords[i][0], coords[i][1]);
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

#ifdef WITH_CAN
    //QtConcurrent::run(getParamsFromCan);
    //Здесь подключаюсь я.
    iodriver = new iodrv(systemState);
    speedAgregator = new SpeedAgregator();

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

    iodriver->start(argv[1], argv[2], (QString(argv[3]).toInt() == 0) ? gps : can);

#else
    QtConcurrent::run(getParamsFromConsole);
#endif


    elMap.load ("./map.gps");
    elMap.setTrackNumber(2);
    for (int i = 0; i < /*496*/ 30; i ++)
    {
        elMap.setMetrometer (coords[i][2]);
        elMap.checkMap (coords[i][0], coords[i][1]);
        //getc(stdin);
    }

    return app->exec();
}
