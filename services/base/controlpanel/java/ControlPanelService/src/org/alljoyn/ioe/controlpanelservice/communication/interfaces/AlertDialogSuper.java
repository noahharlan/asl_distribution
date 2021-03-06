/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Contributed by Qualcomm Connected Experiences, Inc.,
 *    with authorization from the AllSeen Alliance, Inc.
 *    
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    
 *        http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *    
 *    Pursuant to Section 1 of the License, the work of authorship constituting
 *    a Work and any Contribution incorporated in the Work shall mean only that
 *    Contributor's code submissions authored by that Contributor.  Any rights
 *    granted under the License are conditioned upon acceptance of these
 *    clarifications.
 ******************************************************************************/

package org.alljoyn.ioe.controlpanelservice.communication.interfaces;

import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;

/**
 * Super interface for dialog interfaces 
 */
public interface AlertDialogSuper {

	/**
	 * @return Interface version
	 */
	public short getVersion() throws BusException;

	/**
	 * @return States bitmask 
	 * @throws BusException
	 */
	public int getStates() throws BusException; 

	/**
	 * @return Optional parameters
	 * @throws BusException
	 */
	public Map<Short,Variant> getOptParams() throws BusException;
    
	/**
	 * @return Returns the dialog message
	 * @throws BusException
	 */
	public String getMessage() throws BusException;
	
	/**
	 * @return Returns the number of the dialog buttons
	 * @throws BusException
	 */
	public short getNumActions() throws BusException;
	
	/**
	 * Call the method if is relevant
	 */
	public void Action1() throws BusException;
	
	/**
	 * Call the method if is relevant
	 */
	public void Action2() throws BusException;

	/**
	 * Call the method if is relevant
	 */
	public void Action3() throws BusException;

	/**
	 * Signal is sent when the UI container metadata changed 
	 * @param metadata
	 */
	public void MetadataChanged() throws BusException;
}
