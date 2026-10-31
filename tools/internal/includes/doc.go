// Package includes finds and fixes C and C++ files whose includes are not
// grouped and ordered by where the included headers come from.
//
// Where a header comes from is relative to the file that includes it: the
// same library, another library of the repository, a third-party library, or
// the standard library. Libraries are identified by the path that their
// headers are included under, which [Libraries] derives from the layout of the
// repository.
package includes
