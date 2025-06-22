//
//  ViewController.m
//  raptorlauncher
//
//  Created by skynettx on 17.04.23.
//

#import "ViewController.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (IBAction)RunRaptor:(id)sender {
    pid_t childpid;
    childpid = fork();
    
    if (childpid == 0)
    {
        signal(SIGCHLD, SIG_DFL);
        
        const char* pathraptorsetup = "/Contents/MacOS/raptor.sh";
        NSString *mainBundlePath = [[NSBundle mainBundle] bundlePath];
        const char * inputString = [mainBundlePath UTF8String];
        sprintf((char*)inputString,"%s/%s", inputString, pathraptorsetup);
        
        char* arguments[] = { "sh", (char*)inputString, NULL };
        execv("/bin/sh", arguments);
    }
    else
    {
        signal(SIGCHLD, SIG_IGN);
    }
    
}

- (IBAction)RunSetup:(id)sender {
    pid_t childpid;
    childpid = fork();
    
    if (childpid == 0)
    {
        signal(SIGCHLD, SIG_DFL);
        
        const char* pathraptorsetup = "/Contents/MacOS/raptorsetup.sh";
        NSString *mainBundlePath = [[NSBundle mainBundle] bundlePath];
        const char * inputString = [mainBundlePath UTF8String];
        sprintf((char*)inputString,"%s/%s", inputString, pathraptorsetup);
        
        char* arguments[] = { "sh", (char*)inputString, NULL };
        execv("/bin/sh", arguments);
    }
    else
    {
        signal(SIGCHLD, SIG_IGN);
    }
    
}

- (IBAction)Quit:(id)sender {
    exit(0);
    
}

@end
