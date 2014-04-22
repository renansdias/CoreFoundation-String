//
//  main.c
//  CoreFoundation
//
//  Created by Renan Dias on 22/04/2014.
//  Copyright (c) 2014 Renan Dias. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#import <malloc/malloc.h>

char * MyCFStringConversionToCString(CFStringRef);
const char * MyCFStringConversionToCStringFaster(CFStringRef, char **);

int main(int argc, const char * argv[])
{
    // Converting a C String to CFString
    const char * cstring = "Hello World!!!";
    CFStringRef cfString = CFStringCreateWithCString(NULL, cstring, kCFStringEncodingUTF8);
    
    // Showing the CFString
    CFShow(cfString);
    
    // Releasing the CFString
    CFRelease(cfString);
    
    // Converting a CFString to C String. Since it's a bit more complicated
    // we're goig to use a function
    char * cStr = MyCFStringConversionToCString(cfString);
    
    // Checking whether we do have a C String
    if (cStr) {
        printf("cStr: %s\n\n", cStr);
    }
    
    /* ---------------------------------------------------------------------- */
    
    // Converting a CFString to a C String using a faster approach
    CFStringRef cfStringSet[5] = {
        CFSTR("Red"),
        CFSTR("Green"),
        CFSTR("Blue"),
        CFSTR("Black"),
        CFSTR("White"),
    };
    
    // We're going to use and reuse this buffer. We don't know beforehand its
    // size, so let's just assign NULL to it.
    char * buffer = NULL;
    
    const char * cString = NULL;
    
    for (int i = 0; i < 5; i++) {
        cString = MyCFStringConversionToCStringFaster(cfStringSet[i], &buffer);
        printf("cfStringSet[%d]: %s\n", i, cString);
    }
    
    // Why we're freeing buffer and not cString? cString is just a pointer, so we're
    // not responsible for the memory allocated to the string it points to. However,
    // inside MyCFStringConversionToCStringFaster we allocate (or reallocate) space
    // for our buffer, so we need to free it!
    free(buffer);
    
    return 0;
}

/*
 * Converts a CFString into a C String.
 */
char * MyCFStringConversionToCString(CFStringRef cfString) {
    // We'll only do some work if the cfString is not NULL
    if (cfString) {
        // CFIndex is a typedef for signed long
        // length is purely the length of the CFString
        CFIndex length = CFStringGetLength(cfString);
        
        // The maximum number of bytes that a string of length length will take up if encoded
        // in UTF8
        CFIndex maximumSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
        
        char *buffer = (char *) malloc(maximumSize);
        
        // CFStringGetCString returns true if it manages to copy the contents of the CFString into
        // the buffer.
        // If it returns false, it means something went wrong (e.g, not enough space)
        if (CFStringGetCString(cfString, buffer, maximumSize, kCFStringEncodingUTF8)) {
            return buffer;
        }
        
        // Since we couldn't convert the CFString, let's return the space we've allocated
        // before and return NULL to the main method.
        free(buffer);
        return NULL;
    }
    
    // cfString is NULL, it would be pointless to try to convert.
    return NULL;
}

/*
 * Faster way to convert CFString into C String
 */
const char * MyCFStringConversionToCStringFaster(CFStringRef cfString, char **buffer) {
    if (cfString) {
        // Gets a pointer to the C string buffer with the characters of the CFString
        // encoded in UTF8
        const char *str = CFStringGetCStringPtr(cfString, kCFStringEncodingUTF8);
        
        if (!str) {
            CFIndex length = CFStringGetLength(cfString);
            CFIndex maximumSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
            
            // If the current CFString requires more space, let's reallocate.
            if (maximumSize > malloc_size(buffer)) {
                // Remember that buffer is a pointer to an array of chars (i.e, char *).
                // Since our parameter is a pointer TO char *, when need to get back to
                // char * (if we just use buffer, we'd be altering the pointer to the array of chars
                // and not the array itself).
                *buffer = realloc(*buffer, maximumSize);
            }
            
            if (CFStringGetCString(cfString, *buffer, maximumSize, kCFStringEncodingUTF8)) {
                str = *buffer;
            }
        }
        
        return str;
    }
    
    return NULL;
}