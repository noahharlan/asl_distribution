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

namespace AllJoynNET {
partial class SimpleChatForm {
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
        this.txtChat = new System.Windows.Forms.TextBox();
        this.txtInput = new System.Windows.Forms.TextBox();
        this.btnAllJoyn = new System.Windows.Forms.Button();
        this.btnSend = new System.Windows.Forms.Button();
        this.btnClose = new System.Windows.Forms.Button();
        this.txtTranscript = new System.Windows.Forms.TextBox();
        this.SuspendLayout();
        //
        // txtChat
        //
        this.txtChat.Location = new System.Drawing.Point(13, 13);
        this.txtChat.Multiline = true;
        this.txtChat.Name = "txtChat";
        this.txtChat.ReadOnly = true;
        this.txtChat.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
        this.txtChat.Size = new System.Drawing.Size(418, 383);
        this.txtChat.TabIndex = 0;
        //
        // txtInput
        //
        this.txtInput.Location = new System.Drawing.Point(12, 402);
        this.txtInput.Multiline = true;
        this.txtInput.Name = "txtInput";
        this.txtInput.Size = new System.Drawing.Size(419, 147);
        this.txtInput.TabIndex = 1;
        //
        // btnAllJoyn
        //
        this.btnAllJoyn.Location = new System.Drawing.Point(13, 569);
        this.btnAllJoyn.Name = "btnAllJoyn";
        this.btnAllJoyn.Size = new System.Drawing.Size(146, 134);
        this.btnAllJoyn.TabIndex = 2;
        this.btnAllJoyn.Text = "AllJoyn";
        this.btnAllJoyn.UseVisualStyleBackColor = true;
        this.btnAllJoyn.Click += new System.EventHandler(this.btnAllJoyn_Click);
        //
        // btnSend
        //
        this.btnSend.Location = new System.Drawing.Point(183, 579);
        this.btnSend.Name = "btnSend";
        this.btnSend.Size = new System.Drawing.Size(75, 23);
        this.btnSend.TabIndex = 3;
        this.btnSend.Text = "Send";
        this.btnSend.UseVisualStyleBackColor = true;
        this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
        //
        // btnClose
        //
        this.btnClose.Location = new System.Drawing.Point(183, 631);
        this.btnClose.Name = "btnClose";
        this.btnClose.Size = new System.Drawing.Size(75, 23);
        this.btnClose.TabIndex = 4;
        this.btnClose.Text = "Close";
        this.btnClose.UseVisualStyleBackColor = true;
        this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
        //
        // txtTranscript
        //
        this.txtTranscript.Location = new System.Drawing.Point(451, 13);
        this.txtTranscript.Multiline = true;
        this.txtTranscript.Name = "txtTranscript";
        this.txtTranscript.ReadOnly = true;
        this.txtTranscript.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
        this.txtTranscript.Size = new System.Drawing.Size(401, 690);
        this.txtTranscript.TabIndex = 5;
        //
        // SimpleChatForm
        //
        this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(864, 715);
        this.Controls.Add(this.txtTranscript);
        this.Controls.Add(this.btnClose);
        this.Controls.Add(this.btnSend);
        this.Controls.Add(this.btnAllJoyn);
        this.Controls.Add(this.txtInput);
        this.Controls.Add(this.txtChat);
        this.Name = "SimpleChatForm";
        this.Text = "SimpleChatForm";
        this.ResumeLayout(false);
        this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox txtChat;
    private System.Windows.Forms.TextBox txtInput;
    private System.Windows.Forms.Button btnAllJoyn;
    private System.Windows.Forms.Button btnSend;
    private System.Windows.Forms.Button btnClose;
    private System.Windows.Forms.TextBox txtTranscript;
}
}
