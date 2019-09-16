#!/usr/bin/env Rscript

options(width=150)

library(optparse)
options <- list(
        make_option(c("-b","--basepaths"), "store", default=".", help="The list (separated with :) of paths for the sourcefiles", type="character"),
        make_option(c("-o","--output"), "store", default="", help="Second input file", type="character"),
	make_option(c("-q","--quiet"), "store_true", default=FALSE, help="Print only important stuff", type="logical"),
	make_option(c("-s","--silent"), "store_true", default=FALSE, help="Don't print anything", type="logical"),
	make_option(c("-x","--exclude"), "store_true", default=FALSE, help="Exclude dependencies without source file", type="logical"),
	make_option(c("-c","--check"), "store_true", default=FALSE, help="Check if all the dependencies have only a single source code/template file", type="logical")
)

opt <- parse_args(OptionParser(usage="Usage: dep -b path:path:path -o file.mk", options))

if (opt$silent) opt$quiet = TRUE

old_path = getwd()
if (!opt$quiet) cat("Now in path", old_path,"\n");
paths = strsplit(opt$basepaths,":")[[1]]

w = lapply(paths[dir.exists(paths)], function(path) {
  if (!opt$quiet) cat("Checking path:", path,"\n");
  setwd(path)

  f = pipe("grep -o '#[\\t ]*include[\\t ]*\"[^\"]*\"' `find -regex '.*\\(c\\|cu\\|cpp\\|h\\|hpp\\)\\(\\|\\.Rt\\)'` | sed -n 's/^\\([^:]*\\):#[ \\t]*include[ \\t]*\"\\([^\"]*\\)\"/\\1,\\2/gp'")
  w = read.csv(f,col.names=c("file","dep"), stringsAsFactors=FALSE);
  setwd(old_path)

  if (nrow(w) > 0) {
          # make the relative paths in include statement relative to *here*
          w$dep = paste0(sub("[^/]*$","",w$file),w$dep)
          w$path = path
  }
  w
})

w = do.call(rbind,w)
# cut out system includes

#sel = sapply(w[,2],file.exists)
#w = w[sel,,drop=FALSE]

# function reducing the . and ..
resolve.path = function(x) sapply(strsplit(x,"/"),function(x) {
  x = x[! (x %in% c(".",""))];
  while (any(x[-1] == ".." & x[-length(x)] != "..")) {
    i = which(x[-1] == ".." & x[-length(x)] != "..")
    x = x[-c(i,i+1)]
  }
  paste(x,collapse="/")
})

# reduce the paths in w
w$file = gsub(".Rt$","",w$file)
w$file = resolve.path(w$file)
w$dep = resolve.path(w$dep)

w = unique(w)

possible.files = do.call(cbind, sapply( paths, function(path) {
	list( resolve.path(paste0(path,"/",w$dep)), resolve.path(paste0(path,"/",w$dep,".Rt")) )
}))

possible.files.exist = file.exists(possible.files)
dim(possible.files.exist) = dim(possible.files)

w$candidates = apply(ifelse(possible.files.exist, possible.files, NA),1,function(a) paste(a[!is.na(a)],collapse=", "))
w$candidates.num = apply(possible.files.exist, 1, sum)

if (any(w$candidates.num != 1)) {
  if (! opt$silent) {
    wprint = data.frame(Path=w$path, File=w$file, Dependency=w$dep, Candidates = w$candidates, stringsAsFactors=FALSE)
    nf = "Not found"
    if (opt$exclude) nf = paste(nf, "(excluding)")
    wprint$Candidates[w$candidates.num == 0] = nf
    wprint = wprint[w$candidates.num != 1,,drop=FALSE]
    cat("Dependency problems:\n")
    print(wprint, row.names = FALSE)
  }
  if (opt$exclude) w = w[w$candidates.num > 0,]
  if (any(w$candidates.num != 1)) {
    if (opt$check) {
      stop("Some dependencies cannot be connected to a *single* source")
    }
  }
}


dep = do.call(c, as.list( by(w, w$file, function(x) paste(x$file[1],paste(x$dep,collapse=" "),sep=" : ") )))
if (opt$output != "") {
  # write dep.mk
  f=file(opt$output)
  if (length(dep) > 0) {
          cat(paste(dep,"\n#\t@echo \"  DEP        $@ <-- $?\"\n\t@test -f $@ && touch $@\n\n",sep=""),sep="", file=f)
  } else {
          cat("# no dep\n",file=f);
  }
  close(f)
} else {
  cat(paste0(dep,collapse="\n"))
}