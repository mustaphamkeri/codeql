/**
 * @name Arbitrary file write during zip extraction ("Zip Slip")
 * @description Extracting files from a malicious zip archive without validating that the
 *              destination file path is within the destination directory can cause files outside
 *              the destination directory to be overwritten.
 * @kind path-problem
 * @id cs/zipslip
 * @problem.severity error
 * @security-severity 7.5
 * @precision high
 * @tags security
 *       external/cwe/cwe-022
 */

import csharp
import semmle.code.csharp.security.dataflow.ZipSlipQuery
import ZipSlip::PathGraph

from ZipSlip::PathNode source, ZipSlip::PathNode sink
where ZipSlip::flowPath(source, sink)
select source.getNode(), source, sink,
  "Unsanitized archive entry, which may contain '..', is used in a $@.", sink.getNode(),
  "file system operation"
