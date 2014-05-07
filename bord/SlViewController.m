//
//  SlViewController.m
//  bord
//
//  Created by Matthew Thurston on 2014-03-13.
//  Copyright (c) 2014 Slap The Incorporated. All rights reserved.
//

#import "SlViewController.h"
#import "SlMyScene.h"

@implementation SlViewController

@synthesize myImageView,pick_picture_toolbar;

NSString *const SAVED_IMAGE = @"/current_slapping_image.jpg";

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    filtersScrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 300, self.view.bounds.size.width, 90)];
    
    [filtersScrollView setScrollEnabled:YES];
    [filtersScrollView setShowsVerticalScrollIndicator:NO];
    filtersScrollView.showsHorizontalScrollIndicator = NO;
    
    UIBarButtonItem *cameraBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action:@selector(pickPicture:)];
    
    [[self navigationItem] setRightBarButtonItem:cameraBarButtonItem];
    
    [self.view addSubview:filtersScrollView];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
    NSString *docs = [paths objectAtIndex:0];
    NSString* imageName =  [docs stringByAppendingFormat:SAVED_IMAGE];
    NSLog(@"FFF: %@", imageName);
    //./C45C9292-D3DF-4A6D-8AB2-8D7CF426D96B/bord.app/Spaceship.png
    UIImage* imageObj = [[UIImage alloc] initWithContentsOfFile:imageName];
    
    if (imageObj) {
        // set the image vie to the image from file, if it exists (otherwise load default).
        [self.myImageView setImage:imageObj];
    }
}


- (BOOL)shouldAutorotate
{
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return UIInterfaceOrientationMaskAllButUpsideDown;
    } else {
        return UIInterfaceOrientationMaskAll;
    }
}

-(void) pickPicture:(id) sender
{
    UIImagePickerController *imagePickerController = [[UIImagePickerController alloc]init];
    
    NSLog(@"MARFOO1");
    if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
    {
        [imagePickerController setSourceType:UIImagePickerControllerSourceTypeCamera];
    }
    else
    {
        [imagePickerController setSourceType:UIImagePickerControllerSourceTypePhotoLibrary];
    }
    
    [imagePickerController setDelegate:self];
    
    [self presentModalViewController:imagePickerController animated:YES];
}

-(void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    NSLog(@"FOO2");
    finalImage = [info objectForKey:UIImagePickerControllerOriginalImage];
    [self.myImageView setImage:finalImage];
    
    // save image
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
    NSString *docs = [paths objectAtIndex:0];
    NSString* path =  [docs stringByAppendingFormat:SAVED_IMAGE];
    NSLog(@"FFF: %@", path);
    
    NSData* imageData = [NSData dataWithData:UIImageJPEGRepresentation(self.myImageView.image, 80)];
    NSError *writeError = nil;
    BOOL result = [imageData writeToFile:path options:NSDataWritingAtomic error:&writeError];
    
    if(writeError!=nil) {
        NSLog(@"%@: Error saving image: %@", [self class], [writeError localizedDescription]);
    }
    [self dismissModalViewControllerAnimated:YES];
}

@end
