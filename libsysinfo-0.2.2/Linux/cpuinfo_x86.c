/* Re-written from scratch 3 March 2001      */
/* Handles x86 and x86_64 chips on Linux     */
/* by Vince Weaver <vince@deater.net>        */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  /* atof */

#include "../sysinfo.h"
#include "../include/generic.h"

//#define USE_LEGACY_PARSING	1


/******************/
/* AMD chips      */
/******************/

static void fixup_model_amd(struct cpu_info_type *cpu_info,
				char *model_string) {

	strncpy(cpu_info->chip_vendor,"AMD",4);

	/* Clean-up K6 model info */
	if (strstr(model_string,"K6")!=NULL) {
		/* Default to K6 */
		strncpy(cpu_info->chip_type,"K6",3);

		/* Original K6 */
		if ( !(strncmp(model_string,"AMD-K6tm",8)))
			strncpy(cpu_info->chip_type,"K6",3);

		/* Counter-intuitive, but the following are correct */
		if ( !(strncmp(model_string,"AMD-K6(tm)-III",14)))
			strncpy(cpu_info->chip_type,"K6-2+",6);
		else if ( !(strncmp(model_string,"AMD-K6(tm)",10)))
			strncpy(cpu_info->chip_type,"K6-2",5);

		if ( !(strncmp(model_string,"K6-2",4)))
			strncpy(cpu_info->chip_type,"K6-2",5);

		if (strstr(model_string,"3D+")!=NULL) {
			strncpy(cpu_info->chip_type,"K6-III",7);
		}
	}

	/* K5 Chips */
	if (strstr(model_string,"K5")!=NULL) {
		strncpy(cpu_info->chip_type,"K5",3);
	}

	/* Some old K7s were reported as such */
	if (strstr(model_string,"K7")!=NULL) {
		strncpy(cpu_info->chip_type,"Athlon",7);
	}

	/* Athlons */
	if (strstr(model_string,"Athlon(tm) 64 X2")!=NULL) {
		strncpy(cpu_info->chip_type,"Athlon 64 X2",13);
	} else
	if (strstr(model_string,"Athlon(tm) 64")!=NULL) {
		strncpy(cpu_info->chip_type,"Athlon 64",10);
	} else
	if (strstr(model_string,"Athlon")!=NULL) {
		strncpy(cpu_info->chip_type,"Athlon",7);
	}

	/* Specialty Athlons */
	/* Should we support MP too?  */
	/* Should we decode the Speed ratings too (ie, 1600+) */
	if (!strncmp(model_string,"AMD Athlon",10)) {
		if (strstr(model_string,"XP")!=NULL) {
			strncpy(cpu_info->chip_type,"Athlon XP",10);
		}
	}

	/* Durons */
	if (strstr(model_string,"Duron")!=NULL) {
		strncpy(cpu_info->chip_type,"Duron",6);
	}

	/* Unknown */
	if (strstr(model_string,"Unknown")!=NULL) {
		strncpy(cpu_info->chip_type,"Unknown",8);
	}

	/* Sempron */
	if (strstr(model_string,"Sempron")!=NULL) {
		strncpy(cpu_info->chip_type,"Sempron",8);
	}

	/* Turion */
	if (strstr(model_string,"Turion")!=NULL) {
		strncpy(cpu_info->chip_type,"Turion",8);
	}

	/* Opterons */
	if (strstr(model_string,"Opteron")!=NULL) {
		strncpy(cpu_info->chip_type,"Opteron",8);
	}

	/* Phenom */
	if (strstr(model_string,"Phenom")!=NULL) {
		if (strstr(model_string,"II")!=NULL) {
			strncpy(cpu_info->chip_type,"Phenom II",10);
		}
		else {
			strncpy(cpu_info->chip_type,"Phenom",7);
		}
	}

	/* Geode */
	if ( strstr(model_string,"Geode")!=NULL) {
		strncpy(cpu_info->chip_type,"Geode",6);
	}

	/* Handle BIOS/kernel too old issue */
	if ( strstr(model_string,"unknown")!=NULL) {
		strncpy(cpu_info->chip_type,"Unknown",8);
	}

	/* Work around old kernels */
	if (model_string[0]==0) {
		if (cpu_info->model==9) {
			strncpy(cpu_info->chip_type,"K6-III",7);
		}
	}

}


/*********************/
/* Centaur Chips     */
/*********************/

static void fixup_model_centaur(struct cpu_info_type *cpu_info,
				char *model_string) {

	char *processor_begin;

	/* centaur cpuid is fully customizable, but this */
	/* should catch the common case                  */
	strncpy(cpu_info->chip_vendor,"Centaur",8);

	/* Should this be generic?  We should never print processor twice */

	processor_begin=strstr(model_string," Processor");
	if (processor_begin!=NULL) {
		/* I hate to use pointer math */
		model_string[(processor_begin-model_string)]='\0';
	}

	/* Is this the proper thing to do?  It looks silly */
	/* calling them "Centaur VIA" chips                */
	if (strstr(model_string,"VIA")!=NULL) {
		strncpy(cpu_info->chip_vendor,"VIA",4);
		strncpy(cpu_info->chip_type,model_string+4,SYSINFO_CHIP_TYPE_SIZE);
	}

	/* Esther */
	if (strstr(model_string,"Esther")!=NULL) {
		strncpy(cpu_info->chip_type,"Esther",7);
	}

}

/*******************/
/* Cyrix Chips     */
/*******************/
static void fixup_model_cyrix(struct cpu_info_type *cpu_info,
				char *model_string) {

	strncpy(cpu_info->chip_vendor,"Cyrix",6);

	if ( strstr(model_string,"MediaGX")!=NULL) {
		strncpy(cpu_info->chip_type,"MediaGX",8);
	}

	if ( strstr(model_string,"Geode")!=NULL) {
		strncpy(cpu_info->chip_type,"Geode",6);
	}

	if ( !(strncmp(model_string,"6x86",4))) {
		strncpy(cpu_info->chip_type,"6x86",5);
	}
	if ( !(strncmp(model_string,"6x86M",5))) {
		strncpy(cpu_info->chip_type,"6x86MMX",8);
	}
	if ( !(strncmp(model_string,"M III",5))) {
			strncpy(cpu_info->chip_type,"MIII",5);
	}
	else if ( !(strncmp(model_string,"M II",4))) {
			strncpy(cpu_info->chip_type,"MII",4);
	}
}


/*******************/
/* Intel Chips     */
/*******************/

static void fixup_model_intel(struct cpu_info_type *cpu_info,
				char *model_string) {

#ifndef USE_LEGACY_PARSING
	char base_type[BUFSIZ];
	char prefix[BUFSIZ];

	prefix[0]=0;

	strncpy(cpu_info->chip_vendor,"Intel",6);

	/* Old versions of Linux (before 1998?) */
	/* the "model" field was a string not a number */
	if ((cpu_info->family==6) && (cpu_info->model==0)) {
		strncpy(base_type,model_string,BUFSIZ);
		goto too_old;
	}


	/* New, family/model/stepping version */

	printf("Family=%d\n",cpu_info->family);

	if (cpu_info->family==3) {
		strncpy(base_type,"386",4);
	}
	else if (cpu_info->family==4) {
		/* https://en.wikichip.org/wiki/intel/cpuid#Family_4 */
		switch(cpu_info->model) {
			case 1:
				strncpy(base_type,"486 DX",7);
				break;
			case 2:
				strncpy(base_type,"486 SX",7);
				break;
			case 3:
				strncpy(base_type,"486 DX/2",9);
				break;
			case 4:
				strncpy(base_type,"486 SL",7);
				break;
			case 8:
				strncpy(base_type,"486 DX/4",9);
				break;
			default:
				strncpy(base_type,"486",4);
				break;
		}
	}
	else if (cpu_info->family==5) {
		switch(cpu_info->model) {
			case 1: /* P5, P54, P54CQS */
			case 2: /* P54CS  */
				strncpy(base_type,"Pentium",8);
				break;
			case 4: /* P55C -- MMX*/
			case 7: /* P55C Mobile */
			case 8: /* P55C Mobile */
				strncpy(base_type,"Pentium MMX",12);
				break;
			case 9:
			case 10:
				strncpy(base_type,"Quark",6);
				break;
			default:
				strncpy(base_type,"Pentium",8);
				break;
		}
	}
	else if (cpu_info->family==6) {

		/* https://en.wikichip.org/wiki/intel/cpuid */

		/* Thanks, Intel, for this rediculous mess */
		/* Best source of this info is */
		/* arch/x86/include/asm/intel-family.h */
		/* but it's in Hex and uses a lot of silly */
		/* internal Intel lingo */

		/* _L      - mobile parts           */
		/* _G      - with extra graphics    */
		/* _X      - server parts           */
		/* _D      - micro server parts     */
		/* Historical:                      */
		/* _EP     - 2 socket server parts  */
		/* _EX     - 4+ socket server parts */


		switch(cpu_info->model) {
			/* Pentium Pro */
			case 1:
				strncpy(base_type,"Pentium Pro",12);
				break;
			/* Pentium II */
			case 3: /* Klamath */
			case 5: /* Deschutes */
			case 6: /* Celeron Mendocino */
				strncpy(base_type,"Pentium II",11);
				break;
			/* Pentium III */
			case 7: /* Katmai */
			case 8: /* Coppermine */
			case 10:/* Cascades */
			case 11:/* Celeron */
				strncpy(base_type,"Pentium III",12);
				break;
			/* Pentium M */
			case 9:
			case 13:
				strncpy(base_type,"Pentium M",10);
				break;
			/* Core Duo */
			case 14: /* Yonah */
				strncpy(base_type,"Core Duo",9);
				break;
			/* Core2 */
			case 15: /* Merom */
			case 22: /* Merom L */
			case 23: /* Penryn */
			case 29: /* Dunnington */
				strncpy(base_type,"Core2",6);
				break;
			/* Atom */
			case 28: /* Bonnell: Diamondville, Pineview */
			case 38: /* Bonnell Mid: Silverthorne, Lincroft */
			case 54: /* Saltwell: Cedarview */
			case 39: /* Saltwell Mid: Cedarview */
			case 53: /* Saltwell Tabled: Cloverview */
				strncpy(base_type,"Atom",5);
				break;
			case 55: /* Silvermont: Bay Trail, Valleyview */
			case 77: /* Silvermont D: Avaton, Rangely */
			case 74: /* Silvermont Mid: Merriefield */
			case 93: /* 0x5D ?????? */
				strncpy(base_type,"Atom Silvermont",16);
				break;
			case 76: /* Airmont: Cherry Trail, Braswell */
			case 90: /* Airmont Mid: Moorefield */
			case 117:/* Airmont NP: Lightning Mountain */
				strncpy(base_type,"Atom Airmont",13);
				break;
			case 92: /* Goldmont: Apollo Lake */
			case 95: /* Goldmont: Denverton */
				strncpy(base_type,"Atom Goldmont",17);
				break;
			case 122: /* Goldmont Plus: Gemini Lake */
				strncpy(base_type,"Atom Goldmont+",18);
				break;
			case 87:strncpy(base_type,"Knights Landing",16); break;
			case 133:strncpy(base_type,"Knights Mill",13); break;
			/* Nehalem */
			case 30: /* Nehalem */
			case 31: /* G: Auburndale / Havendale */
				strncpy(base_type,"Nehalem",8);
				break;
			case 26: /* EP */
				strncpy(base_type,"Nehalem EP",11);
				break;
			case 46: /* EX */
				strncpy(base_type,"Nehalem EX",11);
				break;
			case 37:
			case 44:strncpy(base_type,"Westmere",9); break;
			case 47:strncpy(base_type,"Westmere EX",12); break;
			case 42:strncpy(base_type,"Sandybridge",12); break;
			case 45:strncpy(base_type,"Sandybridge EP",15); break;
			case 58:strncpy(base_type,"Ivybridge",10); break;
			case 62:strncpy(base_type,"Ivybridge EP",13); break;
			case 60:
			case 70:strncpy(base_type,"Haswell",8); break;
			case 69:strncpy(base_type,"Haswell ULT",12); break;
			case 63:strncpy(base_type,"Haswell EP",11); break;
			case 61:strncpy(base_type,"Broadwell",10); break;
			case 71:strncpy(base_type,"Broadwell-H",12); break;
			case 86:strncpy(base_type,"Broadwell-DE",13); break;
			case 79:strncpy(base_type,"Broadwell EP",13); break;
			case 78:strncpy(base_type,"Skylake Mobile",15); break;
			case 94:strncpy(base_type,"Skylake",8); break;
			case 142:
			case 158:strncpy(base_type,"Kabylake",9); break;
			case 85:
				if (cpu_info->stepping < 5) {
					strncpy(base_type,"Skylake X",10);
				}
				else {
					strncpy(base_type,"Cascadelake X",14);
				}
				break;
			default: strncpy(base_type,"Unknown",8); break;
		}
	}
	else if (cpu_info->family==11) {
		strncpy(base_type,"Knights Corner",15);
	}
	else if (cpu_info->family==15) {
		switch(cpu_info->model) {
			case 0:
			case 1: /* Willamette */
			case 2: /* Northwood */
				strncpy(base_type,"Pentium 4",10);
				break;
			case 3: /* Prescott */
			case 4: /* Prescott */
			case 5:
			case 6:
				strncpy(base_type,"Pentium D",10);
				break;
			default:
				strncpy(base_type,"Unknown",8);
				break;

		}
	}
	else {
		strncpy(base_type,"Unknown",8);
	}

	/* Check for some prefixes */
	if (strstr(model_string,"i3")!=NULL) {
		strncpy(prefix,"i3",3);
	}
	if (strstr(model_string,"i5")!=NULL) {
		strncpy(prefix,"i5",3);
	}
	if (strstr(model_string,"i7")!=NULL) {
		strncpy(prefix,"i7",3);
	}
	if (strstr(model_string,"i9")!=NULL) {
		strncpy(prefix,"i9",3);
	}
	if (strstr(model_string,"Xeon")!=NULL) {
		strncpy(prefix,"Xeon",5);
	}


	/* Construct final */
too_old:
	if (prefix[0]==0) {
		snprintf(cpu_info->chip_type,BUFSIZ,"%s",base_type);
	}
	else {
		snprintf(cpu_info->chip_type,BUFSIZ*2,"%s %s",prefix,base_type);
	}

#else
	/* Handle the various Pentium types */
	if (!(strncmp(model_string,"Pentium",7))) {
		if (strstr(model_string,"75")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium",8);
		}
		if (strstr(model_string,"90")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium",8);
		}
		if (strstr(model_string,"Pro")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium Pro",12);
		}
		if (strstr(model_string,"II")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium II",11);
		}
		if (strstr(model_string,"III")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium III",12);
		}
		if (strstr(model_string,"IV")!=NULL) {
			strncpy(cpu_info->chip_type,"Pentium IV",11);
		}
	}

	/* Now handle the ones with annoying (R) and (TM) */
	if (strstr(model_string,"Pentium(R) M")!=NULL) {
		strncpy(cpu_info->chip_type,"Pentium M",10);
	}
	if (strstr(model_string,"Pentium(R) III")!=NULL) {
		strncpy(cpu_info->chip_type,"Pentium III",12);
	}

	if (strstr(model_string,"Xeon(TM) MP")!=NULL) {
		strncpy(cpu_info->chip_type,"Xeon MP",8);
	}

	/* Atom */
	if (strstr(model_string,"Atom")!=NULL) {
		strncpy(cpu_info->chip_type,"Atom",5);
	}


	/* Should we handle all the various Celeron */
	/* types separately??                       */
	if (strstr(model_string,"Celeron(R) M")!=NULL) {
		strncpy(cpu_info->chip_type,"Celeron M",10);
	}
	else if (strstr(model_string,"Celeron")!=NULL) {
		strncpy(cpu_info->chip_type,"Celeron",8);
	}

	/* Handle values with annoying "Intel(R)" */
	if (strstr(model_string,"Intel(R)")!=NULL) {

		/* Pentium 4 */
		if (strstr(model_string,"Pentium(R) 4")) {
			strncpy(cpu_info->chip_type,"Pentium 4",10);
		}

		/* Pentium D */
		if (strstr(model_string,"Pentium(R) D")) {
			strncpy(cpu_info->chip_type,"Pentium D",10);
		}

		/* Mobile P4 */
		if (strstr(model_string,"Mobile Intel(R) Pentium(R) 4")!=NULL) {
			if (strstr(model_string,"Pentium(R) 4 - M")!=NULL) {
				strncpy(cpu_info->chip_type,"Pentium 4 M",12);
			}
			else {
				strncpy(cpu_info->chip_type,"Mobile Pentium 4",17);
			}
		}

		/* Xeons */
		/* TODO - determine base type based on model id? */
		if ( !(strncmp(model_string,"Intel(R) Xeon(TM) CPU",21)) ||
		     !(strncmp(model_string,"Intel(R) Xeon(R) CPU",20)) || 
		     !(strncmp(model_string,"Intel(R) Genuine CPU",20)) || 
		     !(strncmp(model_string,"Genuine Intel(R) CPU",20)) || 
		     !(strncmp(model_string,"Intel(R) XEON(TM)",17)) ||
		     !(strncmp(model_string,"Intel(R) Xeon(TM)",17)) ) {

			strncpy(cpu_info->chip_type,"Pentium Xeon",16);
		}

		/* Nehalem */
		if (strstr(model_string,"Core(TM) i7")!=NULL) {
			strncpy(cpu_info->chip_type,"i7",3);
		}
		if (strstr(model_string,"Core(TM) i5")!=NULL) {
			strncpy(cpu_info->chip_type,"i5",3);
		}
		if (strstr(model_string,"Core(TM) i3")!=NULL) {
			strncpy(cpu_info->chip_type,"i3",3);
		}

		/* Core and Core2 */
		if (strstr(model_string,"Core(TM)2 Duo")!=NULL) {
			strncpy(cpu_info->chip_type,"Core2 Duo",10);
		}
		else if (strstr(model_string,"Core(TM)2 Quad")!=NULL) {
			strncpy(cpu_info->chip_type,"Core2 Quad",11);
		}
		else if (strstr(model_string,"Core(TM)2")!=NULL) {
			strncpy(cpu_info->chip_type,"Core2",6);
		}	 /* UGH!  I hate this stupid TXXX naming */
		else if ( (strstr(model_string,"T2300")!=NULL) ||
			  (strstr(model_string,"T2400")!=NULL) ||
			  (strstr(model_string,"T2500")!=NULL) ||
			  (strstr(model_string,"T2600")!=NULL)) {
			  strncpy(cpu_info->chip_type,"Core Duo",9);
		}
		else if (strstr(model_string,"E2180")!=NULL) {
			strncpy(cpu_info->chip_type,"Core2 Duo",10);
		}
		else if (strstr(model_string,"2140")!=NULL) {
			strncpy(cpu_info->chip_type,"Core2 Duo",10);
		}
	}

	/* Fix up some older kernels */
	if (model_string[0]==0) {
		if (cpu_info->model==5) {
			strncpy(cpu_info->chip_type,"Pentium II",11);
		}
	}

	if (!strncmp(model_string,"00/07",5)) {
		strncpy(cpu_info->chip_type,"Pentium III",12);
	}

	/* Virtual Machines simulated by QEMU */
	if (strstr(model_string,"QEMU")!=NULL) {
		strncpy(cpu_info->chip_type,"QEMU Virtual",13);
	}
#endif
}


int get_cpu_info(struct cpu_info_type *cpu_info) {

	FILE *fff;
	char temp_string[BUFSIZ];
	char vendor_string[BUFSIZ],model_string[BUFSIZ],plain_model[BUFSIZ];
	int family=0;
	int cpu_count=0;
	float megahertz=0.0,bogomips=0.0;

	vendor_string[0]=model_string[0]=plain_model[0]=0;

	/* We get all of our info here from /proc/cpuinfo */
	fff=fopen(get_cpuinfo_file(),"r");
	if (fff==NULL) {
		return -1;
	}

	while ( (fgets(temp_string,BUFSIZ,fff)!=NULL) ) {

		/* Assume all CPUs in SMP system are the same */
		if (cpu_count==0) {

			if ( !(strncmp(temp_string,"vendor_id",9)) ||
			     !(strncmp(temp_string,"vid",3)) ) { /* 1.2.13 kernels */
				strncpy(vendor_string,parse_line(temp_string),
					BUFSIZ-1);
			}

			if ( !(strncmp(temp_string,"model name",9)) ) {
				strncpy(model_string,parse_line(temp_string),
					BUFSIZ-1);
				clip_lf(model_string,BUFSIZ);
			}
			else {  /* model number */
				if (!(strncmp(temp_string,"model",5))) {
					if (strncmp(parse_line(temp_string),"unknown",7)) {
						strncpy(plain_model,parse_line(temp_string),BUFSIZ-1);
						clip_lf(plain_model,BUFSIZ);
						cpu_info->model=atoi(plain_model);

					}
				}
			}

			if ( !(strncmp(temp_string,"cpu MHz",6))) {
				megahertz=atof(parse_line(temp_string));
			}

			/* We use this for cpus w/o cpuinfo (386s and 486s) */
			if ( !(strncmp(temp_string,"cpu family",10))) {
				cpu_info->family=atoi(parse_line(temp_string));
			}

			/* Sometimes needed to disambiguate */
			if ( !(strncmp(temp_string,"stepping",10))) {
				cpu_info->stepping=atoi(parse_line(temp_string));
			}

			/* Old legacy stuff (2.0.x kernels and earlier) */
			if ( !(strncmp(temp_string,"cpu   ",5))) {
				strncpy(plain_model,parse_line(temp_string),BUFSIZ-1);
				clip_lf(plain_model,255);
			}
		}

		/* Ugh why must people play with capitalization */
		if ( !(strncmp(temp_string,"bogomips",8)) ||
		     !(strncmp(temp_string,"BogoMips",8)) ||
		     !(strncmp(temp_string,"BogoMIPS",8))) {
			bogomips+=atof(parse_line(temp_string));
			/* Cheating way to detect number of intel CPUs */
			cpu_count++;
		}
	}

	fclose(fff);


	/* Re-arrange some of the strings for best results  */
	if (model_string[0]==0) {  /* For 1.2.13 Kernels */
		if (strlen(plain_model)>1) {
			strncpy(model_string,plain_model,BUFSIZ);
		}
	}

	if (model_string[0]==0) {
		strncpy(cpu_info->chip_type,"Unknown",9);
	}
	else {
		strncpy_truncate(cpu_info->chip_type,model_string,SYSINFO_CHIP_TYPE_SIZE);
	}

	strncpy(cpu_info->chip_vendor,"Unknown",9);

	/* sanity */
	if (cpu_count<1) cpu_count=1;

	cpu_info->num_cpus=cpu_count;
	cpu_info->bogomips=bogomips;

	/*********************************************************/
	/* Vendor specific fixups                                */
	/*********************************************************/

	/******************/
	/* AMD chips      */
	/******************/
	if (!strncmp(vendor_string,"AuthenticAMD",12)) {
		fixup_model_amd(cpu_info,model_string);
	}

	/*********************/
	/* Centaur Chips     */
	/*********************/
	if ( !(strncmp(vendor_string,"CentaurHauls",12))) {
		fixup_model_centaur(cpu_info,model_string);
	}


	/*******************/
	/* Cyrix Chips     */
	/*******************/
	if (!strncmp(vendor_string,"CyrixInstead",12)) {
		fixup_model_cyrix(cpu_info,model_string);
	}


	/*******************/
	/* Intel Chips     */
	/*******************/
	if (!strncmp(vendor_string,"GenuineIntel",12)) {
		fixup_model_intel(cpu_info,model_string);
	}

       /*****************/
       /* NexGen        */
       /*****************/
	if ( !(strncmp(vendor_string,"NexGenDriven",12))) {
		strncpy(cpu_info->chip_vendor,"NexGen",7);
	}

	/**************************************/
	/* National Semiconductor Geode Chips */
	/**************************************/
	if ( !(strncmp(vendor_string,"Geode by NSC",12))) {
		strncpy(cpu_info->chip_vendor,"NSC",4);
		strncpy(cpu_info->chip_type,"Geode",6);
	}

	/*****************/
	/* Rise          */
	/*****************/
	if ( !(strncmp(vendor_string,"RiseRiseRise",12))) {
		strncpy(cpu_info->chip_vendor,"Rise",5);
	}

	/******************/
	/* SiS            */
	/******************/
	if ( !(strncmp(vendor_string,"SiS SiS SiS",11))) {
		strncpy(cpu_info->chip_vendor,"SiS",4);
		if (family!=0) {
			sprintf(temp_string,"%i86",family);
			strncpy_truncate(cpu_info->chip_type,temp_string,4);
		}
	}


	/*****************/
	/* Transmeta     */
	/*****************/
	if ( !(strncmp(vendor_string,"GenuineTMx86",12)) ||
	     !(strncmp(vendor_string,"TransmetaCPU",12))) {

		strncpy(cpu_info->chip_vendor,"Transmeta",10);

		if (strstr(model_string,"Crusoe")!=NULL) {
			strncpy(cpu_info->chip_type,"Crusoe",7);
		}
	}

	/******************/
	/* UMC            */
	/******************/
	if ( !(strncmp(vendor_string,"UMC UMC UMC",11))) {
		strncpy(cpu_info->chip_vendor,"UMC",4);

		if (!(strncmp(model_string,"SX",2))) {
			strncpy(cpu_info->chip_type,"486SX",6);
		}
	}

	/******************/
	/* Vortex         */
	/******************/
	if ( !(strncmp(vendor_string,"Vortex86 SoC",11))) {
		strncpy(cpu_info->chip_vendor,"Vortex",7);
		if (family!=0) {
			sprintf(temp_string,"%i86",family);
			strncpy_truncate(cpu_info->chip_type,temp_string,4);
		}
	}

	/**********************************/
	/* Try to handle cpus w/o cpuinfo */
	/**********************************/
	if ( ( !(strncmp(cpu_info->chip_vendor,"Unknown",7))) ||
		( !(strncmp(cpu_info->chip_vendor,"unknown",7)))) {
		if (cpu_info->family!=0) {
			sprintf(temp_string,"%i86",cpu_info->family);
			strncpy_truncate(cpu_info->chip_type,temp_string,4);
		}
		else {
			if (plain_model!=NULL) {
				strncpy_truncate(cpu_info->chip_type,plain_model,SYSINFO_CHIP_TYPE_SIZE);
			}
		}
	}

	cpu_info->megahertz=0.0;

	/* Handle Pretty_printing */
	if (get_pretty_printing()) {
		/* Fix MHz */
		if (megahertz>0.0) {
			cpu_info->megahertz=fix_megahertz(25,megahertz);
		}
	}
	else {
		/* restore RAW vendor string.  Do we want this? */
		strncpy_truncate(cpu_info->chip_type,model_string,64);
		cpu_info->megahertz=megahertz;
 	}

	return 0;
}

	/* Not implemented on ix86 */
int get_hardware_info(char *hardware_string) {

	return 0;
}

	/* Some architectures might have better ways of detecting RAM size */
long long get_arch_specific_mem_size(void) {
	/* We have no special way of detecting RAM */
	return 0;
}
