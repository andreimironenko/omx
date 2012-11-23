#define _concat(p,n) p ## n
#define myPrefixedVar(p,n) _concat(p,n)

/* create a string that can be used at runtime */
char * myPrefixedVar(__xdc_PKGPREFIX, _versionString) = "0, 0, 0";

/* create numeric array that can be used at runtime */
float myPrefixedVar(__xdc_PKGPREFIX, _versionNumArray)[] = {0, 0, 0};
