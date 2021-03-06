/*
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
 */
package org.allseen.lsf.sampleapp;

import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class LampStateViewAdapter implements OnSeekBarChangeListener, OnClickListener {

    public final View stateView;
    public final DimmableItemInfoFragment parentFragment;

    public final Button presetsButton;
    public final SeekBar brightnessSeekBar;
    public final SeekBar hueSeekBar;
    public final SeekBar saturationSeekBar;
    public final SeekBar tempSeekBar;

    private CapabilityData capability;

    public LampStateViewAdapter(View stateView, String tag, DimmableItemInfoFragment parentFragment) {
        this.stateView = stateView;
        this.parentFragment = parentFragment;

        presetsButton = (Button) stateView.findViewById(R.id.stateButton);

        brightnessSeekBar = (SeekBar) stateView.findViewById(R.id.stateSliderBrightness);
        brightnessSeekBar.setTag(tag);
        brightnessSeekBar.setSaveEnabled(false);
        brightnessSeekBar.setOnSeekBarChangeListener(this);
        stateView.findViewById(R.id.stateControlBrightness).setOnClickListener(this);

        hueSeekBar = (SeekBar) stateView.findViewById(R.id.stateSliderHue);
        hueSeekBar.setMax(360);
        hueSeekBar.setTag(tag);
        hueSeekBar.setSaveEnabled(false);
        hueSeekBar.setOnSeekBarChangeListener(this);
        stateView.findViewById(R.id.stateControlHue).setOnClickListener(this);

        saturationSeekBar = (SeekBar) stateView.findViewById(R.id.stateSliderSaturation);
        saturationSeekBar.setTag(tag);
        saturationSeekBar.setSaveEnabled(false);
        saturationSeekBar.setOnSeekBarChangeListener(this);
        stateView.findViewById(R.id.stateControlSaturation).setOnClickListener(this);

        tempSeekBar = (SeekBar) stateView.findViewById(R.id.stateSliderColorTemp);
        tempSeekBar.setMax(DimmableItemScaleConverter.VIEW_COLORTEMP_SPAN);
        tempSeekBar.setTag(tag);
        tempSeekBar.setSaveEnabled(false);
        tempSeekBar.setOnSeekBarChangeListener(this);
        stateView.findViewById(R.id.stateControlColorTemp).setOnClickListener(this);

        capability = new CapabilityData();
    }

    public void setCapability(CapabilityData capability) {
        this.capability = capability;
        boolean displayStarFooter = false;

        // dimmable
        if (capability.dimmable >= CapabilityData.SOME) {
            brightnessSeekBar.setEnabled(true);
            presetsButton.setEnabled(true);
        } else {
            brightnessSeekBar.setEnabled(false);
            presetsButton.setEnabled(false);
            parentFragment.setTextViewValue(stateView, R.id.stateTextBrightness, parentFragment.getResources().getString(R.string.na), 0);
        }
        if (capability.dimmable == CapabilityData.SOME) {
            stateView.findViewById(R.id.stateLabelBrightnessStar).setVisibility(View.VISIBLE);
            displayStarFooter = true;
        } else {
            stateView.findViewById(R.id.stateLabelBrightnessStar).setVisibility(View.INVISIBLE);
        }

        // color support
        if (capability.color >= CapabilityData.SOME) {
            hueSeekBar.setEnabled(true);
            saturationSeekBar.setEnabled(true);
        } else {
            hueSeekBar.setEnabled(false);
            saturationSeekBar.setEnabled(false);
            parentFragment.setTextViewValue(stateView, R.id.stateTextHue, parentFragment.getResources().getString(R.string.na), 0);
            parentFragment.setTextViewValue(stateView, R.id.stateTextSaturation, parentFragment.getResources().getString(R.string.na), 0);
        }
        if (capability.color == CapabilityData.SOME) {
            stateView.findViewById(R.id.stateLabelHueStar).setVisibility(View.VISIBLE);
            stateView.findViewById(R.id.stateLabelSaturationStar).setVisibility(View.VISIBLE);
            displayStarFooter = true;
        } else {
            stateView.findViewById(R.id.stateLabelHueStar).setVisibility(View.INVISIBLE);
            stateView.findViewById(R.id.stateLabelSaturationStar).setVisibility(View.INVISIBLE);
        }

        // temperature support
        if (capability.temp >= CapabilityData.SOME) {
            tempSeekBar.setEnabled(true);
        } else {
            tempSeekBar.setEnabled(false);
            parentFragment.setTextViewValue(stateView, R.id.stateTextColorTemp, parentFragment.getResources().getString(R.string.na), 0);
        }
        if (capability.temp == CapabilityData.SOME) {
            stateView.findViewById(R.id.stateLabelColorTempStar).setVisibility(View.VISIBLE);
            displayStarFooter = true;
        } else {
            stateView.findViewById(R.id.stateLabelColorTempStar).setVisibility(View.INVISIBLE);
        }

        if (displayStarFooter) {
            stateView.findViewById(R.id.stateTextNotSupportedByAll).setVisibility(View.VISIBLE);
        } else {
            stateView.findViewById(R.id.stateTextNotSupportedByAll).setVisibility(View.GONE);
        }

        saturationCheck();
    }

    public void setPreset(String presetName) {
        presetsButton.setText(presetName);
    }

    public void setBrightness(long modelBrightness, boolean uniformBrightness) {
        if (capability.dimmable >= CapabilityData.SOME) {
            int viewBrightness = DimmableItemScaleConverter.convertBrightnessModelToView(modelBrightness);

            brightnessSeekBar.setProgress(viewBrightness);
            brightnessSeekBar.setThumb(parentFragment.getResources().getDrawable(uniformBrightness ? R.drawable.slider_thumb_normal : R.drawable.slider_thumb_midstate));

            parentFragment.setTextViewValue(stateView, R.id.stateTextBrightness, viewBrightness, R.string.units_percent);
        }
    }

    public void setHue(long modelHue, boolean uniformHue) {
        if (capability.color >= CapabilityData.SOME) {
            int viewHue = DimmableItemScaleConverter.convertHueModelToView(modelHue);

            hueSeekBar.setProgress(viewHue);
            hueSeekBar.setThumb(parentFragment.getResources().getDrawable(uniformHue ? R.drawable.slider_thumb_normal : R.drawable.slider_thumb_midstate));

            parentFragment.setTextViewValue(stateView, R.id.stateTextHue, viewHue, R.string.units_degrees);
        }
    }

    public void setSaturation(long modelSaturation, boolean uniformSaturation) {
        if (capability.color >= CapabilityData.SOME) {
            int viewSaturation = DimmableItemScaleConverter.convertSaturationModelToView(modelSaturation);

            saturationSeekBar.setProgress(viewSaturation);
            saturationSeekBar.setThumb(parentFragment.getResources().getDrawable(uniformSaturation ? R.drawable.slider_thumb_normal : R.drawable.slider_thumb_midstate));

            parentFragment.setTextViewValue(stateView, R.id.stateTextSaturation, viewSaturation, R.string.units_percent);

            saturationCheck();
        }
    }

    public void setColorTemp(long modelColorTemp, boolean uniformColorTemp) {
        if (capability.temp >= CapabilityData.SOME) {
            int viewColorTemp = DimmableItemScaleConverter.convertColorTempModelToView(modelColorTemp);

            tempSeekBar.setProgress(viewColorTemp - DimmableItemScaleConverter.VIEW_COLORTEMP_MIN);
            tempSeekBar.setThumb(parentFragment.getResources().getDrawable(uniformColorTemp ? R.drawable.slider_thumb_normal : R.drawable.slider_thumb_midstate));

            parentFragment.setTextViewValue(stateView, R.id.stateTextColorTemp, viewColorTemp, R.string.units_kelvin);
        }
    }

    private void saturationCheck() {
        if (saturationSeekBar.getProgress() == 0) {
            hueSeekBar.setEnabled(false);
        } else if (capability.color >= CapabilityData.SOME) {
            hueSeekBar.setEnabled(true);
        }

        if (saturationSeekBar.getProgress() == saturationSeekBar.getMax()) {
            tempSeekBar.setEnabled(false);
        } else if (capability.temp >= CapabilityData.SOME) {
            tempSeekBar.setEnabled(true);
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        //AJSI-291: UI Slider behaviour change (from continuous updating to updating when finger is lifted)
        /*if (parentFragment != null) {
            setTextViewValues(seekBar);
            if (fromUser) {
                parentFragment.setField(seekBar);
            }
        }*/
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        // Currently nothing to do
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (parentFragment.parent != null) {
            parentFragment.setField(seekBar);

            if (seekBar.getId() == R.id.stateSliderSaturation) {
                saturationCheck();
            }

            setTextViewValues(seekBar);
        }
    }

    private void setTextViewValues(SeekBar seekBar) {
        if (parentFragment.parent != null) {
            int seekBarID = seekBar.getId();
            int progress = seekBar.getProgress();

            // if it is a status slider, update indicator text
            if (seekBarID == R.id.stateSliderBrightness) {
                parentFragment.setTextViewValue(stateView, R.id.stateTextBrightness, progress, R.string.units_percent);
            } else if (seekBarID == R.id.stateSliderHue) {
                parentFragment.setTextViewValue(stateView, R.id.stateTextHue, progress, R.string.units_degrees);
            } else if (seekBarID == R.id.stateSliderSaturation) {
                parentFragment.setTextViewValue(stateView, R.id.stateTextSaturation, progress, R.string.units_percent);
            } else if (seekBarID == R.id.stateSliderColorTemp) {
                parentFragment.setTextViewValue(stateView, R.id.stateTextColorTemp, progress + DimmableItemScaleConverter.VIEW_COLORTEMP_MIN, R.string.units_kelvin);
            }
        }
    }

    @Override
    public void onClick(View v) {
        int viewID = v.getId();
        
        if (viewID == R.id.stateControlBrightness) {
            if (capability.dimmable <= CapabilityData.NONE) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.no_support_dimmable);
            }
        } else if (viewID == R.id.stateControlHue) {
            if (capability.color <= CapabilityData.NONE) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.no_support_color);
            } else if (saturationSeekBar.getProgress() == 0) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.saturation_disable_hue);
            }
        } else if (viewID == R.id.stateControlSaturation) {
            if (capability.color <= CapabilityData.NONE) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.no_support_color);
            }
        } else if (viewID == R.id.stateControlColorTemp) {
            if (capability.temp <= CapabilityData.NONE) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.no_support_temp);
            } else if (saturationSeekBar.getProgress() == saturationSeekBar.getMax()) {
            	((SampleAppActivity)parentFragment.getActivity()).showToast(R.string.saturation_disable_temp);
            }
        }
    }  
}

  

