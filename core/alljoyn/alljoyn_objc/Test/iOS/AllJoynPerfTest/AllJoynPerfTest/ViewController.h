////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

#import <UIKit/UIKit.h>
#import "PerformanceStatistics.h"

@interface ViewController : UITableViewController

@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UITextView *eventsTextView;
@property (weak, nonatomic) IBOutlet UIProgressView *progressView;
@property (weak, nonatomic) IBOutlet UISegmentedControl *transportTypeSegmentedControl;
@property (weak, nonatomic) IBOutlet UISegmentedControl *messageTypeSegmentedControl;
@property (weak, nonatomic) IBOutlet UISegmentedControl *messageSizeSegmentedControl;
@property (weak, nonatomic) IBOutlet UISegmentedControl *operationTypeSegmentedControl;
@property (weak, nonatomic) IBOutlet UILabel *statusTextField1;
@property (weak, nonatomic) IBOutlet UILabel *statusTextField2;

@property (nonatomic, strong) PerformanceStatistics *performanceStatistics;

- (IBAction)didTouchStartButton:(id)sender;
- (void)displayPerformanceStatistics;
- (void)resetPerformanceStatistics;

@end
