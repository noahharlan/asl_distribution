/******************************************************************************
* Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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
package org.alljoyn.aroundme.Groups;




import org.alljoyn.aroundme.R;
import org.alljoyn.aroundme.Adapters.GroupAdapter;
import org.alljoyn.aroundme.MainApp.AppConstants;
import org.alljoyn.bus.annotation.BusSignal;
import org.alljoyn.devmodules.api.groups.GroupsAPI;
import org.alljoyn.devmodules.api.groups.GroupsListener;
import org.alljoyn.devmodules.common.GroupDescriptor;
import org.alljoyn.devmodules.common.GroupListDescriptor;
import org.alljoyn.devmodules.common.Utilities;
import org.alljoyn.storage.GroupCache;

import android.app.Activity;
import android.content.Intent;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Html;
import android.text.Spanned;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;



public class GroupTestActivity extends Activity {



	private static final String TAG = "GroupTestActivity";

	private        UIhandler  mUI ;

	private static Button     mTestButton ;
	private static TextView   mResult;
	private static GroupsAPI  mGroupsAPI;


	/* Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// load layout
		setContentView(R.layout.grouptest);

		// get references to display items
		mTestButton = (Button)   findViewById(R.id.test);
		mResult     = (TextView) findViewById(R.id.result);


		// Create the UI Handler
		mUI = new UIhandler ();

		// create the GroupsAPI object
		mGroupsAPI = new GroupsAPI();
		
		// Declare callbacks for GroupsAPI
		mGroupsAPI.registerListener(new GroupsListener(){

			@Override
			public void onGroupInvitation(String group, String originator) {
				Log.v(TAG, "onGroupInvitation("+group+", "+originator+")");
			}

			@Override
			public void onGroupInvitationAccepted(String group, String id) {
				Log.v(TAG, "onGroupInvitationAccepted("+group+", "+id+")");
			}

			@Override
			public void onGroupInvitationRejected(String group, String id) {
				Log.v(TAG, "onGroupInvitationRejected("+group+", "+id+")");
			}

			@Override
			public void onGroupInvitationTimeout(String group) {
				Log.v(TAG, "onGroupInvitationTimeout("+group+")");
			}

			@Override
			public void onGroupAdded(String group) {
				Log.v(TAG, "onGroupAdded("+group+")");
			}

			@Override
			public void onGroupRemoved(String group) {
				Log.v(TAG, "onGroupRemoved("+group+")");
			}

			@Override
			public void onGroupActive(String group) {
				Log.v(TAG, "onGroupActive("+group+")");
			}

			@Override
			public void onGroupInactive(String group) {
				Log.v(TAG, "onGroupInactive("+group+")");
			}

			@Override
			public void onGroupEnabled(String group) {
				Log.v(TAG, "onGroupEnabled("+group+")");
			}

			@Override
			public void onGroupDisabled(String group) {
				Log.v(TAG, "onGroupDisabled("+group+")");
			}


			@Override
			public void onGroupMemberJoined(String group, String id) {
				Log.v(TAG, "onGroupMemberJoined("+group+", "+id+")");
			}

			@Override
			public void onGroupMemberLeft(String group, String id) {
				Log.v(TAG, "onGroupMemberLeft("+group+", "+id+")");
			}

			@Override
			public void onTestResult(String results) {
				Log.v(TAG, "onTestResult()");
				mUI.showResults(results);
			}

		});

		// Handler for Run Tests button
		mTestButton.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				mUI.startTest();
			}
		});

	}// onCreate()



	/* Called when the activity is exited. */
	@Override
	protected void onDestroy() {
		super.onDestroy();
	}


	/////////////////////////////////////////////////////////////////////////////////
	// Menu Handling
	/////////////////////////////////////////////////////////////////////////////////

	// None, right now



	/////////////////////////////////////////////////////////////////////////////////
	// UI Handler Message Queue Thread
	// Initiate all UI-related functions through accessor methods for this
	/////////////////////////////////////////////////////////////////////////////////

	private class UIhandler{


		// List of UI commands
		private static final int UI_START_TEST       =  1; 
		private static final int UI_SHOW_RESULTS     =  2; 

		// Accessor Methods for handler

		// Start a named group as owner
		public void startTest(){
			MainUIHandler.sendEmptyMessage(UI_START_TEST);	
		}

		// Stop a group I own
		public void showResults(String results){
			Message msg = MainUIHandler.obtainMessage(UI_SHOW_RESULTS);
			msg.obj=results;
			MainUIHandler.sendMessage(msg);	
		}

		// Main UI Handler
		private Handler MainUIHandler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				switch (msg.what) {
				case UI_START_TEST: {
					mGroupsAPI.runGroupsTest();
					mResult.setText("Test running, takes about 20 seconds...");
					break;
				}
				case UI_SHOW_RESULTS: {
					String result = (String)msg.obj;
					Spanned spanned = Html.fromHtml(result);
					mResult.setText(spanned);
					break;
				}
				default: {
					Toast.makeText(getApplicationContext(), TAG+"ERR: Unkown UI command", Toast.LENGTH_LONG).show();
					break;
				}
				}//switch
			}
		};

	}//UIhandler

} // end of Activity
