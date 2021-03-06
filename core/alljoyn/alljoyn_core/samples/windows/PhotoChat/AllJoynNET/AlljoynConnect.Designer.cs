﻿// ****************************************************************************
// Copyright AllSeen Alliance. All rights reserved.
//
//    Contributed by Qualcomm Connected Experiences, Inc.,
//    with authorization from the AllSeen Alliance, Inc.
//    
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//    
//        http://www.apache.org/licenses/LICENSE-2.0
//    
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//    
//    Pursuant to Section 1 of the License, the work of authorship constituting
//    a Work and any Contribution incorporated in the Work shall mean only that
//    Contributor's code submissions authored by that Contributor.  Any rights
//    granted under the License are conditioned upon acceptance of these
//    clarifications.
// ******************************************************************************

namespace PhotoChat {
partial class AllJoynConnect {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null)) {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        this.label2 = new System.Windows.Forms.Label();
        this.txtSession = new System.Windows.Forms.TextBox();
        this.rbAdvertise = new System.Windows.Forms.RadioButton();
        this.rbJoin = new System.Windows.Forms.RadioButton();
        this.txtHandle = new System.Windows.Forms.TextBox();
        this.label1 = new System.Windows.Forms.Label();
        this.btnCancel = new System.Windows.Forms.Button();
        this.btnOk = new System.Windows.Forms.Button();
        this.button1 = new System.Windows.Forms.Button();
        this.SuspendLayout();
        //
        // label2
        //
        this.label2.AutoSize = true;
        this.label2.Location = new System.Drawing.Point(16, 58);
        this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        this.label2.Name = "label2";
        this.label2.Size = new System.Drawing.Size(75, 13);
        this.label2.TabIndex = 13;
        this.label2.Text = "Session Name";
        //
        // txtSession
        //
        this.txtSession.Location = new System.Drawing.Point(95, 58);
        this.txtSession.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.txtSession.Name = "txtSession";
        this.txtSession.Size = new System.Drawing.Size(188, 20);
        this.txtSession.TabIndex = 12;
        //
        // rbAdvertise
        //
        this.rbAdvertise.AutoSize = true;
        this.rbAdvertise.Checked = true;
        this.rbAdvertise.Location = new System.Drawing.Point(19, 91);
        this.rbAdvertise.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.rbAdvertise.Name = "rbAdvertise";
        this.rbAdvertise.Size = new System.Drawing.Size(87, 17);
        this.rbAdvertise.TabIndex = 11;
        this.rbAdvertise.TabStop = true;
        this.rbAdvertise.Text = "Start Session";
        this.rbAdvertise.UseVisualStyleBackColor = true;
        //
        // rbJoin
        //
        this.rbJoin.AutoSize = true;
        this.rbJoin.Location = new System.Drawing.Point(115, 91);
        this.rbJoin.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.rbJoin.Name = "rbJoin";
        this.rbJoin.Size = new System.Drawing.Size(84, 17);
        this.rbJoin.TabIndex = 10;
        this.rbJoin.Text = "Join Session";
        this.rbJoin.UseVisualStyleBackColor = true;
        //
        // txtHandle
        //
        this.txtHandle.Location = new System.Drawing.Point(95, 20);
        this.txtHandle.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.txtHandle.Name = "txtHandle";
        this.txtHandle.Size = new System.Drawing.Size(188, 20);
        this.txtHandle.TabIndex = 9;
        //
        // label1
        //
        this.label1.AutoSize = true;
        this.label1.Location = new System.Drawing.Point(16, 20);
        this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
        this.label1.Name = "label1";
        this.label1.Size = new System.Drawing.Size(66, 13);
        this.label1.TabIndex = 8;
        this.label1.Text = "Your Handle";
        //
        // btnCancel
        //
        this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnCancel.Location = new System.Drawing.Point(212, 127);
        this.btnCancel.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(71, 19);
        this.btnCancel.TabIndex = 15;
        this.btnCancel.Text = "Cancel";
        this.btnCancel.UseVisualStyleBackColor = true;
        //
        // btnOk
        //
        this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
        this.btnOk.Location = new System.Drawing.Point(19, 127);
        this.btnOk.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.btnOk.Name = "btnOk";
        this.btnOk.Size = new System.Drawing.Size(71, 19);
        this.btnOk.TabIndex = 14;
        this.btnOk.Text = "Connect";
        this.btnOk.UseVisualStyleBackColor = true;
        //
        // button1
        //
        this.button1.Location = new System.Drawing.Point(115, 127);
        this.button1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.button1.Name = "button1";
        this.button1.Size = new System.Drawing.Size(71, 19);
        this.button1.TabIndex = 16;
        this.button1.Text = "Setup";
        this.button1.UseVisualStyleBackColor = true;
        //
        // AlljoynConnect
        //
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(302, 168);
        this.Controls.Add(this.button1);
        this.Controls.Add(this.btnCancel);
        this.Controls.Add(this.btnOk);
        this.Controls.Add(this.label2);
        this.Controls.Add(this.txtSession);
        this.Controls.Add(this.rbAdvertise);
        this.Controls.Add(this.rbJoin);
        this.Controls.Add(this.txtHandle);
        this.Controls.Add(this.label1);
        this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
        this.Name = "AlljoynConnect";
        this.Text = "AlljoynConnect";
        this.ResumeLayout(false);
        this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox txtSession;
    private System.Windows.Forms.RadioButton rbAdvertise;
    private System.Windows.Forms.RadioButton rbJoin;
    private System.Windows.Forms.TextBox txtHandle;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button btnCancel;
    private System.Windows.Forms.Button btnOk;
    private System.Windows.Forms.Button button1;
}
}
