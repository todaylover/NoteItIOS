//
//  CPTokeniser.h
//  CoreParse
//
//  Created by Tom Davie on 10/02/2011.
//  Copyright 2011 In The Beginning... All rights reserved.
//

#import <Foundation/Foundation.h>

#import "CPTokenRecogniser.h"
#import "CPTokenStream.h"

@class CPTokeniser;

/**
 * The delegate of a CPTokeniser must adopt the CPTokeniserDelegate protocol.  This allows for customising if/when a tokeniser should produce a token.
 */
@protocol CPTokeniserDelegate <NSObject>

/** 
 * Determines whether a CPTokeniser should produce a token and consume the associated input string.
 * 
 * If this method returns NO, the CPTokeniser continues to attempt to recognise tokens at the same point in the input string with other token recognisers.
 * 
 * @param tokeniser The CPTokeniser attempting to produce the token.
 * @param token The CPToken that the tokeniser has recognised.
 * @return Return YES if the tokeniser should consume the token, NO otherwise.
 */
- (BOOL)tokeniser:(CPTokeniser *)tokeniser shouldConsumeToken:(CPToken *)token;

/**
 * Allows you to replace a taken in the tokeniser's output stream.
 *
 * @param tokeniser The CPTokeniser that will produce the token.
 * @param token The CPToken that the tokeniser has recognised.
 * @return Return an array of CPToken objects to place in the output token stream.
 */
- (NSArray *)tokeniser:(CPTokeniser *)tokeniser willProduceToken:(CPToken *)token;

@optional

/**
 * This method is called when no recogniser matches a token at the current position in the input stream.  You must provide a new location in the input stream to start
 * tokenising from again, or NSNotFound to stop the tokeniser.  You may optionally provide an error message to attach to a CPErrorToken.
 *
 * @param tokeniser The CPTokeniser that could not match any token in the input stream.
 * @param input The input given to the tokeniser.
 * @param position The position in the input stream at which the tokeniser failed.
 * @param errorMessage A pointer to which you can write to provide an error message to output in an error token.
 * @return Return a new position in the input to begin tokenising from.  Return NSNotFound to request that the tokeniser stops.
 */
- (NSUInteger)tokeniser:(CPTokeniser *)tokeniser didNotFindTokenOnInput:(NSString *)input position:(NSUInteger)position error:(NSString **)errorMessage;

@end

/**
 * The CPTokeniser class provides tokenisation of NSStrings into CPTokenStreams, and describes what kinds of tokens to produce given particular string inputs.
 *   
 * Tokenisers are built up by adding a list of CPTokenRecogniser objects to the CPTokeniser.
 * Each one recognises a different token.  Each recogniser is given a chance to match a token in priority order.
 * When a recogniser matches a token, the -tokeniser:shouldConsumeToken: delegate method is called.
 * If this method returns NO, the rest of the recognisers are tried in priority order.
 * If it returns YES, -tokeniser:willProduceToken: is called, and the resulting array of tokens added to
 * the output stream.
 */
@interface CPTokeniser : NSObject <NSCoding>

///---------------------------------------------------------------------------------------
/// @name Managing the Delegate
///---------------------------------------------------------------------------------------

/**
 * The object that acts as a delegate to the receiving CPTokeniser.
 */
@property (readwrite, assign) id<CPTokeniserDelegate> delegate;

///---------------------------------------------------------------------------------------
/// @name Managing recognised tokens
///---------------------------------------------------------------------------------------

/**
 * Adds a token recogniser at the end of the priority list of recognisers.
 *
 * @param recogniser The token recogniser to add.  This value must not be `nil`.
 * @exception Raises an `NSInvalidArgumentException` if recogniser is `nil`.
 * @see insertTokenRecogniser:atPriority:
 * @see insertTokenRecogniser:beforeRecogniser:
 */
- (void)addTokenRecogniser:(id<CPTokenRecogniser>)recogniser;

/**
 * Inserts a given token recogniser at a given priority level in the tokeniser.
 *
 * The recogniser currently at that priority and all those below it move downwards.
 * 
 * @param recogniser The token recogniser to insert.  This value must not be `nil`.
 * @param priority The priority level to insert at.
 * @exception Raises an `NSInvalidArgumentException` if recogniser is `nil`.
 * @exception Raises an `NSRangeException` if priority is greater than the number of token recognisers in the tokeniser.
 * @see addTokenRecogniser:
 * @see insertTokenRecogniser:beforeRecogniser:
 */
- (void)insertTokenRecogniser:(id<CPTokenRecogniser>)recogniser atPriority:(NSInteger)priority;

/**
 * Inserts a given token recogniser before another.
 *
 * The recogniser currently at that priority and all those below it move downwards.
 * 
 * @param recogniser The token recogniser to insert.  This value must not be `nil`.
 * @param other The token recogniser to insert before.
 * @exception Raises an `NSInvalidArgumentException` if recogniser is `nil` or if other is not in the tokeniser's priority queue.
 * @see addTokenRecogniser:
 * @see insertTokenRecogniser:atPriority:
 */
- (void)insertTokenRecogniser:(id<CPTokenRecogniser>)recogniser beforeRecogniser:(id<CPTokenRecogniser>)other;

/**
 * Removes all occurances of recogniser in the tokeniser's priority list.
 *
 * @param recogniser The token recogniser to remove.
 * @see addTokenRecogniser:
 */
- (void)removeTokenRecogniser:(id<CPTokenRecogniser>)recogniser;

///---------------------------------------------------------------------------------------
/// @name Tokenising
///---------------------------------------------------------------------------------------

/**
 * Tokenises an input string by repeatedly using the recognisers in the tokeniser's priority list.
 *
 * If the entire input is tokenised a CPEOFToken is added to the end of the result token stream.  If not, the token stream ends with no EOF token.
 * 
 * @param input The input string to tokenise.
 * @return Returns a token stream containing all tokens found in the input string.
 */
- (CPTokenStream *)tokenise:(NSString *)input;

/**
 * Tokenises an input string into a pre-allocated output CPTokenStream.
 *
 * If the entire input is tokenised a CPEOFToken is added to the end of the result token stream.  If not, the token stream ends with no EOF token.
 * This method can be useful for multithreading parsers, allowing you to create the token stream that the tokeniser writes to and pass it to both tokeniser and parser threads.
 * 
 * @param input The input string to tokenise.
 * @param tokenStream The token stream to add tokens to from the input string.
 */
- (void)tokenise:(NSString *)input into:(CPTokenStream *)tokenStream;

@end
