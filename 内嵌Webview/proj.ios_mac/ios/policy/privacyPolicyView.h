
#ifndef privacyPolicyView_hpp
#define privacyPolicyView_hpp
#include <functional>
#import <Foundation/NSString.h>
#import "RootViewController.h"

@interface privacyPolicyView:NSObject <UIWebViewDelegate>

- (id)init:(RootViewController *)view;

- (void)loadView:(NSString *)url title:(NSString *)title;

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType;
- (void)webViewDidStartLoad:(UIWebView *)webView;
- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;

- (void)btnClick:(id)sender;

@property(nonatomic, strong)RootViewController* rootViewC;
@property(nonatomic, strong)UIView *privacyView;
@property(nonatomic, strong)UIWebView *webView;
@end

#endif /* our_privacyPolicyView_hpp */
