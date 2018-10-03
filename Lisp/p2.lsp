(defun rozdil (lst1 lst2)
  (if (eq lst1 nil)
    (if (eq lst2 nil)
      nil
      (cons (car lst2) (rozdil lst1 (cdr lst2)))
    )  
    (if (eq lst2 nil)
      (cons (car lst1) (rozdil (cdr lst1) lst2))
      (cons (- (car lst1) (car lst2)) (rozdil (cdr lst1) (cdr lst2)))
    )
  )  
)

(defun obsahuje (lst el)
  (if (eq lst nil)
    nil
    (if (equal (car lst) el)
      T
      (obsahuje (cdr lst) el)
    )
  )
)


(defun smazW (lstSrc lstDst)
	(smaz lstSrc lstDst '())

)


(defun smaz (lstSrc lstDst lstLeave)
  (if (eq lstSrc nil)
    nil
    (if (listp (car lstSrc))
      (cons (smaz (car lstSrc) lstDst lstLeave) (smaz (cdr lstSrc) lstDst lstLeave))
      (if  (obsahuje lstDst (car lstSrc))
	(if  (obsahuje lstLeave (car lstSrc))
          (smaz (cdr lstSrc) lstDst lstLeave)
	  (cons (car lstSrc) (smaz (cdr lstSrc) lstDst (cons (car lstSrc) lstLeave) ))
	)
        (cons (car lstSrc) (smaz (cdr lstSrc) lstDst lstLeave))
      )
    )   
  )
)


