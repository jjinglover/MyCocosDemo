
#include "privacyPolicyView.h"
#include "AppController.h"

static privacyPolicyView *instance;

@implementation privacyPolicyView
-(id)init:(RootViewController *)view
{
    self = [super init];
    instance=self;
    self.rootViewC=view;
    return self;
}
- (void)loadView:(NSString *)url title:(NSString *)title
{
    CGRect screenRect=[UIScreen mainScreen].applicationFrame;
    _privacyView=[[UIView alloc]initWithFrame:screenRect];
    _privacyView.backgroundColor=[UIColor colorWithRed:1 green:1 blue:1 alpha:1];
    [self.rootViewC.view addSubview:_privacyView];
    
    int offsetY=40,lineHight=2;
    CGRect rect=screenRect;
    rect.origin.y=rect.origin.y+offsetY-lineHight;
    rect.size.height=lineHight;
    UIView *lineView=[[UIView alloc]initWithFrame:rect];
    lineView.backgroundColor=[UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:1];
    [_privacyView addSubview:lineView];
    
    rect=screenRect;
    rect.origin.y=rect.origin.y+offsetY;
    rect.size.height=screenRect.size.height-offsetY;
    _webView = [[UIWebView alloc] initWithFrame:rect];
    NSURL *urlS=[NSURL URLWithString:url];
    [_webView loadRequest:[NSURLRequest requestWithURL:urlS]];
    [_privacyView addSubview:_webView];
    [_webView setScalesPageToFit:YES];
    _webView.delegate=self;
    
    UIButton *backbtn = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    backbtn.frame = CGRectMake(screenRect.size.width-60, 5, 30, 30);
    UIImage *btnImg=[UIImage imageNamed:@"zRes_back.png"];
    [backbtn setBackgroundImage:btnImg forState:UIControlStateNormal];
    [backbtn setBackgroundImage:btnImg forState:UIControlStateHighlighted];
    [backbtn addTarget:self action:@selector(btnClick:) forControlEvents:UIControlEventTouchUpInside];
    [_privacyView addSubview:backbtn];
    
    rect=CGRectMake(20, 0, 300, 40);
    UILabel *lbl=[[UILabel alloc]initWithFrame:rect];
    lbl.font=[UIFont systemFontOfSize:20];
    lbl.text=title;
    [_privacyView addSubview:lbl];
}

- (void)btnClick:(id)sender
{
    if (_privacyView) {
        [_privacyView removeFromSuperview];
        _privacyView=nullptr;
    }
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    
    
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    
    
}
@end

NSString * toNSStr(std::string str)
{
    NSString *str_ns = [[NSString alloc] initWithCString:str.c_str()
                                                encoding:NSUTF8StringEncoding];
    return str_ns;
}

void loadPrivacyPolicyView(const char* url,const char* title)
{
    if (instance) {
        [instance loadView:toNSStr(url) title:toNSStr(title)];
    }
}


