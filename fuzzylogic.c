/*
 * fuzzylogic.c
 *
 *  Created on: Jun 25, 2015
 *      Author: Bryce
 */

/* This is my implementation of a general fuzzy logic control system with a variable number of inputs
 * and outputs, membership functions, and rules. Membership functions can be any shape defineable
 * by 2 points and 2 slopes -- trapezoids, triangles, etc. Rules can have any number of antecedents
 * and outpus, which can vary from rule to rule. In order to compute membership strength, a "min" and
 * "max" function will be used.
 *
 * System: Solar Powered Windows
 * Inputs: 1
 * 		ADC Value between 0 and 12,400 that represents how much sun is incident upon the solar panel
 * Outputs: 1
 * 		Scaled DAC value that represents how much voltage will be sent to electrochromic film (more volatage = more transparent)
 * Membership Functions: 5
 *
 */
#include <stdint.h>
#include <stdio.h>
#define MAX_NAME 10 // max number of characters in names
#define UPPER_LIMIT 255 // max strength of a membership function


extern int32_t TXData;	// SPI transmission data

/* Membership functions are associated with each system input and output */
struct mf_type {
	uint8_t name[MAX_NAME]; 		// name of the membership function
	int32_t value;                  // degree of membership/output strength
	int32_t startVal;				// leftmost x-axis point of membership function
	int32_t ascSlope; 				// slope of left side of membership function
	int32_t endVal;					// rightmost x-axis point of membership function
	int32_t dscSlope;				// slope of right side of membership function
	struct mf_type * next; 			// pointer to the next membership function
};

/* each rule has an IF side and a THEN side. Elements that make up the IF side are pointers to the values inside of
 * the mf_type structs. The THEN side consists of pointers to the output strength values inside of the mf_types.
 * Each rule contains a pointer to the next rule in the rule base
 */
struct rule_element_type {
	int * value;						// pointer to previous output strength value
	struct rule_element_type * next; 	// next output element in the rule
};

struct rule_type {
	struct rule_element_type * if_side;		// list of antecedents in rule
	struct rule_element_type * then_side; 	// list of outputs in rule
	struct rule_type * next; 				// next rule in the rule base
};

struct rule_type * Rule_Base;				// list of all the rules in rule base, will need to declare this
											// will need to create this as a static list
struct mf_type * Membership_Functions;		// list of all the membership functions that we are using

/*************************************** HELPER FUNCTIONS ***************************************/

int32_t min(int32_t x, int32_t y) {
	return x <= y ? x : y;
}
int32_t max(int32_t x, int32_t y) {
	return x >= y ? x : y;
}

/* Compute Area of Trapezoid -- Good helper function to have, in case each inference pass produces
 * a new set of output strengths which affect the areas of trapezoidal membership.
 */
int32_t compute_area_of_trapezoid(struct mf_type * mf) {
    // Initialization
	int16_t run1;
    int16_t run2;
    int16_t base;
    int16_t top;
    int16_t area;

    // calculate area
	base = mf->endVal - mf->startVal;
	run1 = mf->value/mf->ascSlope;
	run2 = mf->value/mf->dscSlope;
	top = base - run1 - run2;
	return (mf->value * (base + top) / 2);
}

/*************************************** PUBLIC FUNCTIONS ***************************************/

/* Compute Degree of Membership -- Degree to which input val is a member of my current membersip function
 * 1. Compute Delta terms to determine if input is inside or outside my membership functions
 * 2. If outside, then degree of membership is 0. Otherwise smaller of delta_1 * slope1 and delta2 * slope2
 * 3. Enforce upper limit
 */
int32_t compute_degree_of_membership(struct mf_type * mf, int32_t input) {
	// Initialiation
	int16_t status = 0;
	int32_t delta1, delta2;
	delta1 = input - mf->startVal;
	delta2 = mf->endVal - input;
	if((delta1 <= 0) || (delta2 <= 0)) {	// input outside membership function
		mf->value = 0;
		status = 1;
	}
	else {
		mf->value = min((mf->ascSlope * delta1), (mf->dscSlope * delta2));
		mf->value = min(mf->value, UPPER_LIMIT); 		// enforce upper limit
		status = 1;
	}
	return status;
}

/* Fuzzification-- Degree of membership value is calculated for each membership function of each system input.
 * Values correspond to antecedents in rules
 */
int32_t fuzzification(int32_t crispVal) {
	// Initialization
	struct mf_type * mf = Membership_Functions;		// membership function pointer, initializes to first membership function
	for(; mf != NULL; mf = mf->next) {
		compute_degree_of_membership(mf, crispVal);
	}
	return 1;
}

/* Rule Evaluation -- Each rule consists of a list of pointers to antecedents(IF side), list of pointers to outputs(THEN side),
 * and a pointer to the next rule in Rule_Base. When a rule is evaluated, its antecedents are ANDed together using a min function
 * to form the strength of the rule. The strength of each rule is applied to each of the listed rule outputs. If an output has
 * already been assigned a rule strength, the if will be OR'ed and a max function will be used to determine the resulting strength
 */
int32_t rule_evaluation(void) {
	struct rule_type * rule;
	struct rule_element_type * if_ptr;
	struct rule_element_type * then_ptr;
	rule = Rule_Base;
	int32_t strength;

	// iterate through the rule base
	for(; rule != NULL; rule = rule->next) {
		strength = UPPER_LIMIT; 			// max rule strength allowed
		if_ptr = rule->if_side;
		then_ptr = rule->then_side;
		// process if-side of rule to determine strength
		for(; if_ptr != NULL; if_ptr = if_ptr->next) {
			strength = min(strength, *(if_ptr->value));
		}
		// process then-side of rule to apply strength
		for(; then_ptr != NULL; then_ptr = then_ptr->next) {
			*(then_ptr->value) = max(strength, *(then_ptr->value));
		}
	}

	return 1;	// success
}

/*
 * Defuzzification -- converts all of of the fuzzified output values into a crisp output
 * by using the weighted average of all of the fuzzified values.
 */
int32_t defuzzification() {
	struct mf_type * mf;			// output membership function pointer
	int32_t sum_of_products = 0; 		// sum of products of area and centroid
	int32_t sum_of_areas = 0;			// sum of shortend trapezoid area
	int32_t area = 0;
	int32_t centroid = 0;

	mf = Membership_Functions;
	// compute a defuzzified value for each system output
	for(; mf != NULL; mf = mf->next) {
		area = compute_area_of_trapezoid(mf);
		centroid = mf->startVal + (mf->endVal - mf->startVal) / 2;
		sum_of_products += area * centroid;
		sum_of_areas += area;
	}

	return (sum_of_products / sum_of_areas);	// weighted average
}


//*****************************************************************************
//
//! Uses fuzzy logic to convert value coming in from the ADC into an output 
//! value for the DAC 
//
//*****************************************************************************
int32_t convert_to_system_output(int32_t adcVal) {
	fuzzification(adcVal);
	rule_evaluation();
	TXData = defuzzification();
    return 1;               // success
}
