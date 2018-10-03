(defun delka (lst arr)
  (if (eq lst nil)
    arr
    (if (listp (car lst))
      (if (sudy (car lst)) 
	(delka  (cdr lst) (cons (+ 1 (car arr) ) (cdr arr)))
	(delka  (cdr lst) (cons (+ 1 (car arr) ) '(1)))
      )
	(delka (cdr lst) (cons (+ 1 (car arr) ) (cdr arr)))
  )
)
)

(defun sudy (lst)
 (if  (and (evenp (car (delka lst '(0 0 ) ))  )  (equalp (car (cdr (delka lst '(0 0 )))) 0) )
     T
     nil
  )
)
 
