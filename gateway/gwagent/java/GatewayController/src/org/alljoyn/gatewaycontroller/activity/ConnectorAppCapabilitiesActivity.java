/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

package org.alljoyn.gatewaycontroller.activity;

import java.lang.reflect.Method;

import org.alljoyn.gatewaycontroller.CallbackMethod;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApp;
import org.alljoyn.gatewaycontroller.sdk.ConnectorCapabilities;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v13.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;

/**
 * The activity presents the {@link ConnectorCapabilities} and the manifest file of a
 * {@link ConnectorApp}.
 */
public class ConnectorAppCapabilitiesActivity extends BaseActivity implements ActionBar.TabListener {
    public static final String TAG = "gwcapp" + ConnectorAppCapabilitiesActivity.class.getSimpleName();

    /**
     * Loads the fragment of the selected tab
     */
    public class SelectedPageAdapter extends FragmentPagerAdapter {

        /**
         * Constructor
         *
         * @param fragmentMgr
         */
        public SelectedPageAdapter(FragmentManager fragmentMgr) {

            super(fragmentMgr);
        }

        /**
         * @see android.support.v13.app.FragmentPagerAdapter#getItem(int)
         */
        @Override
        public Fragment getItem(int position) {

            Fragment frg = null;

            switch (position) {

                case RULES_TAB_INDEX: {

                    frg = ConnectorAppCapabilitiesFragment.createInstance(connectorCapabilities);
                    break;
                }
                case FILE_TAB_INDEX: {

                    frg = ConnectorAppCapabilitiesFileFragment.createInstance(manifestFile);
                    break;
                }
            }

            return frg;
        }

        /**
         * @see android.support.v4.view.PagerAdapter#getItemPosition(java.lang.Object)
         */
        @Override
        public int getItemPosition(Object object) {

            return POSITION_NONE;
        }

        /**
         * @see android.support.v4.view.PagerAdapter#getCount()
         */
        @Override
        public int getCount() {

            return manifestTabs.length;
        }
    }

    // =========================================================//

    private static int[] manifestTabs = new int[] { R.string.connector_app_manifest_rules,
                                                    R.string.connector_app_manifest_file };

    private static final int RULES_TAB_INDEX = 0;
    private static final int FILE_TAB_INDEX  = 1;

    /**
     * Used for the page swiping
     */
    private ViewPager viewPager;

    /**
     * Loads the fragment of the selected page
     */
    private SelectedPageAdapter pageAdapter;

    /**
     * Asynchronous task to be executed
     */
    private AsyncTask<Void, Void, Void> asyncTask;

    /**
     * Reflection of the retrieveData method
     */
    private static Method retrieveDataMethod;

    /**
     * Invoke this method when onSessionJoined event is called
     */
    private CallbackMethod invokeOnSessionReady;

    /**
     * The manifest file
     */
    private String manifestFile;

    /**
     * The {@link ConnectorCapabilities}
     */
    private ConnectorCapabilities connectorCapabilities;

    static {

        try {

            Class<ConnectorAppCapabilitiesActivity> activClass = ConnectorAppCapabilitiesActivity.class;
            retrieveDataMethod = activClass.getDeclaredMethod("retrieveData");
        } catch (NoSuchMethodException nsme) {
            Log.wtf(TAG, "NoSuchMethodException", nsme);
        }
    }

    /**
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connector_application_manifest);

        if (retrieveDataMethod == null) {

            Log.e(TAG, "Reflection of the required methods is undefined, can't continue working");
            showOkDialog("Error", "Internal application error./n Can't continue working.", "Ok", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
        }
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onStart()
     */
    @Override
    protected void onStart() {

        super.onStart();

        // Check existence of the selected gateway app
        if (app.getSelectedGatewayApp() == null) {

            Log.w(TAG, "Selected gateway has been lost, handling");
            handleLostOfGateway();
            return;
        }

        setTitle(getString(R.string.title_activity_connector_manifest) + ": " +
                 app.getSelectedConnectorApp().getFriendlyName());

        retrieveData();
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onStop()
     */
    @Override
    protected void onStop() {

        super.onStop();

        if (asyncTask != null) {
            asyncTask.cancel(true);
        }
    }

    /**
     * @see android.app.ActionBar.TabListener#onTabSelected(android.app.ActionBar.Tab,
     *      android.app.FragmentTransaction)
     */
    @Override
    public void onTabSelected(Tab tab, FragmentTransaction ft) {

        viewPager.setCurrentItem(tab.getPosition());
    }

    /**
     * @see android.app.ActionBar.TabListener#onTabReselected(android.app.ActionBar.Tab,
     *      android.app.FragmentTransaction)
     */
    @Override
    public void onTabReselected(Tab tab, FragmentTransaction ft) {
    }

    /**
     * @see android.app.ActionBar.TabListener#onTabUnselected(android.app.ActionBar.Tab,
     *      android.app.FragmentTransaction)
     */
    @Override
    public void onTabUnselected(Tab tab, FragmentTransaction ft) {
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSessionJoined()
     */
    @Override
    protected void onSessionJoined() {

        super.onSessionJoined();

        if (invokeOnSessionReady == null) {
            Log.w(TAG, "onSessionJoined is called, but invokeOnSessionReady is undefiend, returning");
            return;
        }

        try {
            invokeOnSessionReady.getMethod().invoke(this, invokeOnSessionReady.getArgs());
        } catch (Exception e) {
            Log.e(TAG, "Failed to invoke the method: '" + invokeOnSessionReady.getMethod().getName() + "'");
            showOkDialog("Error", "Failed to execute an operation", "Ok", null);
        }

        invokeOnSessionReady = null;
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSelectedGatewayLost()
     */
    @Override
    protected void onSelectedGatewayLost() {

        super.onSelectedGatewayLost();
        handleLostOfGateway();
    }

    /**
     * Loads Tab navigation
     */
    private void loadTabNavigation() {

        final ActionBar actionBar = getActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);

        actionBar.setHomeButtonEnabled(false);
        actionBar.setDisplayHomeAsUpEnabled(false);

        pageAdapter = new SelectedPageAdapter(getFragmentManager());
        viewPager   = (ViewPager) findViewById(R.id.connectorAppManifestPager);
        viewPager.setAdapter(pageAdapter);

        viewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                actionBar.setSelectedNavigationItem(position);
            }
        });

        // Fill the ActionBar with tabs
        for (int nameRes : manifestTabs) {

            actionBar.addTab(actionBar.newTab().setText(getString(nameRes)).setTabListener(this));
        }
    }

    /**
     * Retrieves the {@link ConnectorCapabilities} and the manifest file of the selected
     * {@link ConnectorApp}
     */
    private void retrieveData() {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't retrieve Connector Application manifest, no session with the GW is established, waiting for" +
                           " the onSessionJoined event");

            invokeOnSessionReady = new CallbackMethod(retrieveDataMethod, new Object[] {});
            return;
        }

        showProgressDialog("Retrieving manifest data");

        Log.d(TAG, "Retrieving manifest data");

        asyncTask = new AsyncTask<Void, Void, Void>() {

            private boolean isOk = true;

            @Override
            protected Void doInBackground(Void... params) {

                try {

                    ConnectorApp selConnApp = app.getSelectedConnectorApp();
                    manifestFile            = selConnApp.retrieveManifestFile(sid);
                    connectorCapabilities   = selConnApp.retrieveConnectorCapabilities(sid);
                } catch (GatewayControllerException gce) {

                    isOk = false;
                    Log.e(TAG, "Failed to retrieve the manifest", gce);
                }
                return null;
            }

            @Override
            protected void onPostExecute(Void result) {

                hideProgressDialog();

                if (!isOk) {

                    showOkDialog("Error", "Failed to retrieve the manifest data", "Ok", null);
                    return;
                }

                loadTabNavigation();
            }// onPostExecute
        };
        asyncTask.execute();

    }// retrieveData

}
