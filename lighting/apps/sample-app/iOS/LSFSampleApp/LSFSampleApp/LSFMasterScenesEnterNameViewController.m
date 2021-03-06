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

#import "LSFMasterScenesEnterNameViewController.h"
#import "LSFMasterSceneMembersTableViewController.h"
#import "LSFUtilityFunctions.h"
#import <LSFSDKLightingDirector.h>

@interface LSFMasterScenesEnterNameViewController ()

-(void)leaderModelChangedNotificationReceived:(NSNotification *)notification;

@end

@implementation LSFMasterScenesEnterNameViewController

@synthesize pendingMasterScene = _pendingMasterScene;
@synthesize nameTextField = _nameTextField;

-(void)viewDidLoad
{
    [super viewDidLoad];
}

-(void)viewWillAppear: (BOOL)animated
{
    [super viewWillAppear: animated];
    [self.nameTextField becomeFirstResponder];
    [self.navigationController.toolbar setHidden: YES];

    //Set notification handler
    [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(leaderModelChangedNotificationReceived:) name: @"LSFContollerLeaderModelChange" object: nil];
}

-(void)viewWillDisappear: (BOOL)animated
{
    [super viewWillDisappear: animated];
    [self.navigationController.toolbar setHidden: NO];

    //Clear notification handler
    [[NSNotificationCenter defaultCenter] removeObserver: self];
}

-(void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

/*
 * Notification Handlers
 */
-(void)leaderModelChangedNotificationReceived:(NSNotification *)notification
{
    LSFSDKController *leaderModel = [notification.userInfo valueForKey: @"leader"];
    if (![leaderModel connected])
    {
        [self dismissViewControllerAnimated: YES completion: nil];
    }
}

/*
 * UITextFieldDelegate implementation
 */
-(BOOL)textFieldShouldReturn: (UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

/*
 * UIAlertViewDelegate implementation
 */
-(void)alertView: (UIAlertView *)alertView clickedButtonAtIndex: (NSInteger)buttonIndex
{
    if (buttonIndex == 0)
    {
        [alertView dismissWithClickedButtonIndex: 0 animated: NO];
    }

    if (buttonIndex == 1)
    {
        [alertView dismissWithClickedButtonIndex: 1 animated: NO];
        [self.nameTextField resignFirstResponder];
        [self performSegueWithIdentifier: @"ChooseMasterSceneMembers" sender: self];
    }
}

/*
 * Cancel Button Handler
 */
-(IBAction)cancelButtonPressed: (id)sender
{
    [self dismissViewControllerAnimated: YES completion: nil];
}

-(IBAction)nextButtonPressed: (id)sender
{
    if ([self.nameTextField.text isEqualToString: @""])
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle: @"No Master Scene Name"
                                                        message: @"You need to provide a master scene name in order to proceed."
                                                       delegate: self
                                              cancelButtonTitle: @"OK"
                                              otherButtonTitles: nil];
        [alert show];

        return;
    }

    BOOL nameMatches = [[[[LSFSDKLightingDirector getLightingDirector] masterScenes] valueForKeyPath: @"name"] containsObject: self.nameTextField.text];
    if (nameMatches)
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle: @"Duplicate Name"
                                                        message: [NSString stringWithFormat: @"Warning: there is already a master scene named \"%@.\" Although it's possible to use the same name for more than one master scene, it's better to give each master scene a unique name.\n\nKeep duplicate master scene name \"%@\"?", self.nameTextField.text, self.nameTextField.text]
                                                       delegate: self
                                              cancelButtonTitle: @"NO"
                                              otherButtonTitles: @"YES", nil];
        [alert show];
        return;
    }
    else if (![LSFUtilityFunctions checkNameLength: self.nameTextField.text entity:@"Master Scene Name"])
    {
        return;
    }
    else if (![LSFUtilityFunctions checkWhiteSpaces: self.nameTextField.text entity:@"Master Scene Name"])
    {
        return;
    }
    else
    {
        [self performSegueWithIdentifier: @"ChooseMasterSceneMembers" sender: self];
    }
}

/*
 * Segue Methods
 */
-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    self.pendingMasterScene.name = self.nameTextField.text;

    if ([segue.identifier isEqualToString: @"ChooseMasterSceneMembers"])
    {
        LSFMasterSceneMembersTableViewController *msmtvc = [segue destinationViewController];
        msmtvc.pendingMasterScene = self.pendingMasterScene;
        msmtvc.usesCancel = NO;
    }
}

@end
