/*
 * (C) Copyright IBM Corp. 2004, 2005
 * Copyright (c) 2005, Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 * Author(s):
 *     Kevin Gao <kevin.gao@intel.com>
 *     Carl McAdams <carlmc@us.ibm.com>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiSensorReadingGet
 * Description:   
 *   Check if the sensor doesn't provide a reading, reading.IsSupported is set
 *   to false
 * Line:        P80-28:P80-30
 */
#include <stdio.h>
#include "saf_test.h"

int Test_Rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id,
	     SaHpiRdrT rdr)
{
	SaHpiSensorReadingT reading;
	SaHpiSensorNumT num;
	SaErrorT status;
	int ret = SAF_TEST_UNKNOWN;
	SaHpiBoolT SensorEnabled;

	if (rdr.RdrType == SAHPI_SENSOR_RDR) {
		//
		// Is this sensor disabled?
		//
		status = saHpiSensorEnableGet(session_id,
					      resource_id,
					      rdr.RdrTypeUnion.SensorRec.Num,
					      &SensorEnabled);
		if (status != SA_OK) {
			e_print(saHpiSensorEnableGet, SA_OK, status);
			ret = SAF_TEST_UNRESOLVED;
		} else {
			if (SensorEnabled == SAHPI_FALSE)
				// when the sensor is disabled we wont use it
				ret = SAF_TEST_NOTSUPPORT;

			if (!rdr.RdrTypeUnion.SensorRec.DataFormat.IsSupported) {
				ret = SAF_TEST_NOTSUPPORT;
			}
		}
	} else			// Non-Sensor RDR
		ret = SAF_TEST_NOTSUPPORT;

	if (ret == SAF_TEST_UNKNOWN) {
		num = rdr.RdrTypeUnion.SensorRec.Num;
		status = saHpiSensorReadingGet(session_id, resource_id, num,
					       &reading, NULL);

		if (status != SA_OK) {
			if (status == SA_ERR_HPI_ENTITY_NOT_PRESENT) {
				ret = SAF_TEST_NOTSUPPORT;
			} else {
				e_print(saHpiSensorReadingGet, SA_OK, status);
				ret = SAF_TEST_UNRESOLVED;
			}
		} else if (reading.IsSupported ==
			   rdr.RdrTypeUnion.SensorRec.DataFormat.IsSupported) {
			// When successful
			ret = SAF_TEST_PASS;
		} else {
			m_print("reading.IsSupported should be False "
				"when sensor doesn't support reading.");
			ret = SAF_TEST_FAIL;
		}
	}

	return ret;
}

int Test_Resource(SaHpiSessionIdT session,
		  SaHpiRptEntryT report, callback2_t func)
{
	int retval = SAF_TEST_UNKNOWN;

	if (report.ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)
		retval = do_resource(session, report, func);
	else			//Resource Does not support Sensors
		retval = SAF_TEST_NOTSUPPORT;
	return (retval);
}

/**********************************************************
*   Main Function
*      takes no arguments
*      
*       returns: SAF_TEST_PASS when successfull
*                SAF_TEST_FAIL when an unexpected error occurs
*************************************************************/
int main()
{
	int ret = SAF_TEST_UNKNOWN;

	ret = process_all_domains(&Test_Resource, &Test_Rdr, NULL);

	return ret;
}
