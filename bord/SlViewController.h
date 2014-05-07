//
//  SlViewController.h
//  bord
//

//  Copyright (c) 2014 Slap The Incorporated. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <SpriteKit/SpriteKit.h>

@interface SlViewController : UIViewController<UINavigationControllerDelegate, UIImagePickerControllerDelegate>
{
    UIScrollView *filtersScrollView;
    UIImage *finalImage;
}


@property (strong, nonatomic) IBOutlet UIImageView *myImageView;

@property (strong, nonatomic) IBOutlet UIToolbar *pick_picture_toolbar;

@end
