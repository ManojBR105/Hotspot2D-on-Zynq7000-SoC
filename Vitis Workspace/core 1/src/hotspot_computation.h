#include "constants.h"

#define MAX_PD (3.0e5)
/* required precision in degrees */
const float PRECISION =  0.01;
#define SPEC_HEAT_SI 1.75e3
#define K_SI 1000
/* capacitance fitting factor */
const float FACTOR_CHIP =  0.5;


/* chip parameters */
const float t_chip = 0.0005;
const float chip_height = 0.016;
const float chip_width = 0.016;

/* ambient temperature, assuming no package at all */
const float amb_temp = 300.0;

/*end of start of hotspot software calculation related variables*/
const int row = INPUT_SIZE;
const int col = INPUT_SIZE;

const float grid_height = chip_height / row;
const float grid_width = chip_width / col;

const float Cap = FACTOR_CHIP * SPEC_HEAT_SI * t_chip * grid_width * grid_height;
const float Rx = grid_width / (2.0 * K_SI * t_chip * grid_height);
const float Ry = grid_height / (2.0 * K_SI * t_chip * grid_width);
const float Rz = t_chip / (K_SI * grid_height * grid_width);

const float max_slope = MAX_PD / (FACTOR_CHIP * t_chip * SPEC_HEAT_SI);
const float step = PRECISION / max_slope / 1000.0;

const float Rx_1=1.f/Rx;
const float Ry_1=1.f/Ry;
const float Rz_1=1.f/Rz;
const float Cap_1 = step/Cap;

const float cns = Ry_1*Cap_1;
const float cwe = Rx_1*Cap_1;
const float ctb = Rz_1*Cap_1;
const float camb = (ctb * amb_temp);
const float cc = (1.0-((2.0 * cns) + (2.0 * cwe) + ctb));





void single_iteration(float *result, float *temp, float *power);
