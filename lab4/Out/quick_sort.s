.data
_prompt: .asciiz "Entering an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra
write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra
quicksort:
	addi $sp,  $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	addi $sp,  $sp, -4
	addi $sp,  $sp, -4
	addi $sp,  $sp, -4
	addi $sp,  $sp, -4
	lw $t0, 12($fp)
	lw $t1, 16($fp)
	sw $t0, 12($fp)
	sw $t1, 16($fp)
	bge $t0,  $t1, label0
	lw $t0, -12($fp)
	lw $t1, 12($fp)
	move $t0, $t1
	lw $t2, -4($fp)
	move $t2, $t1
	lw $t3, -8($fp)
	lw $t4, 16($fp)
	move $t3, $t4
	sw $t0, -12($fp)
	sw $t1, 12($fp)
	sw $t2, -4($fp)
	sw $t3, -8($fp)
	sw $t4, 16($fp)
label2:
	lw $t0, -4($fp)
	lw $t1, -8($fp)
	sw $t0, -4($fp)
	sw $t1, -8($fp)
	bge $t0,  $t1, label1
label4:
	lw $t1, -4($fp)
	li $t2, 4
	mul $t0,  $t1, $t2
	lw $t4, 8($fp)
	add $t3,  $t4, $t0
	lw $t5, 0($t3)
	lw $t7, -12($fp)
	mul $t6,  $t7, $t2
	add $t8,  $t4, $t6
	lw $t9, 0($t8)
	sw $t1, -4($fp)
	sw $t4, 8($fp)
	addi $sp,  $sp, -4
	sw $t5, -20($fp)
	sw $t7, -12($fp)
	addi $sp,  $sp, -4
	sw $t8, -24($fp)
	addi $sp,  $sp, -4
	sw $t9, -28($fp)
	bgt $t5,  $t9, label3
	lw $t0, -4($fp)
	lw $t1, 16($fp)
	sw $t0, -4($fp)
	sw $t1, 16($fp)
	bge $t0,  $t1, label3
	lw $t0, -4($fp)
	li $t1, 1
	add $t0,  $t0, $t1
	sw $t0, -4($fp)
	j label4
label3:
label6:
	lw $t1, -8($fp)
	li $t2, 4
	mul $t0,  $t1, $t2
	lw $t4, 8($fp)
	add $t3,  $t4, $t0
	lw $t5, 0($t3)
	lw $t7, -12($fp)
	mul $t6,  $t7, $t2
	add $t8,  $t4, $t6
	lw $t9, 0($t8)
	sw $t1, -8($fp)
	sw $t4, 8($fp)
	addi $sp,  $sp, -4
	sw $t5, -32($fp)
	sw $t7, -12($fp)
	addi $sp,  $sp, -4
	sw $t8, -36($fp)
	addi $sp,  $sp, -4
	sw $t9, -40($fp)
	ble $t5,  $t9, label5
	lw $t0, -8($fp)
	li $t1, 1
	sub $t0,  $t0, $t1
	sw $t0, -8($fp)
	j label6
label5:
	lw $t0, -4($fp)
	lw $t1, -8($fp)
	sw $t0, -4($fp)
	sw $t1, -8($fp)
	bge $t0,  $t1, label7
	lw $t1, -4($fp)
	li $t2, 4
	mul $t0,  $t1, $t2
	lw $t4, 8($fp)
	add $t3,  $t4, $t0
	lw $t5, -16($fp)
	lw $t5, 0($t3)
	mul $t6,  $t1, $t2
	add $t7,  $t4, $t6
	lw $t9, -8($fp)
	mul $t8,  $t9, $t2
	add $s0,  $t4, $t8
	lw $s1, 0($s0)
	sw $s1, 0($t7)
	mul $s2,  $t9, $t2
	add $s3,  $t4, $s2
	sw $t5, 0($s3)
	sw $t1, -4($fp)
	sw $t4, 8($fp)
	sw $t5, -16($fp)
	sw $t9, -8($fp)
	addi $sp,  $sp, -4
	sw $s3, -44($fp)
label7:
	j label2
label1:
	lw $t1, -12($fp)
	li $t2, 4
	mul $t0,  $t1, $t2
	lw $t4, 8($fp)
	add $t3,  $t4, $t0
	lw $t5, -16($fp)
	lw $t5, 0($t3)
	mul $t6,  $t1, $t2
	add $t7,  $t4, $t6
	lw $t9, -8($fp)
	mul $t8,  $t9, $t2
	add $s0,  $t4, $t8
	lw $s1, 0($s0)
	sw $s1, 0($t7)
	mul $s2,  $t9, $t2
	add $s3,  $t4, $s2
	sw $t5, 0($s3)
	li $s5, 1
	sub $s4,  $t9, $s5
	sw $t1, -12($fp)
	sw $t4, 8($fp)
	sw $t5, -16($fp)
	sw $t9, -8($fp)
	addi $sp,  $sp, -4
	sw $s4, -48($fp)
	lw $t0, -48($fp)
	addi $sp,  $sp, -4
	sw $t0, 0($sp)
	lw $t1, 12($fp)
	addi $sp,  $sp, -4
	sw $t1, 0($sp)
	lw $t2, 8($fp)
	addi $sp,  $sp, -4
	sw $t2, 0($sp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal quicksort
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	move $t0, $v0
	lw $t2, -8($fp)
	li $t3, 1
	add $t1,  $t2, $t3
	addi $sp,  $sp, -4
	sw $t1, -52($fp)
	sw $t2, -8($fp)
	lw $t0, 16($fp)
	addi $sp,  $sp, -4
	sw $t0, 0($sp)
	lw $t1, -52($fp)
	addi $sp,  $sp, -4
	sw $t1, 0($sp)
	lw $t2, 8($fp)
	addi $sp,  $sp, -4
	sw $t2, 0($sp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal quicksort
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	move $t0, $v0
	addi $sp,  $sp, -4
	sw $t0, -56($fp)
label0:
	li $t0, 0
	move $v0, $t0
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
main:
	addi $sp,  $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	addi $sp,  $sp, -4
	addi $sp,  $sp, -20
	addi $sp,  $sp, -4
	lw $t0, -28($fp)
	la $t0, -24($fp)
	li $t2, 0
	li $t3, 4
	mul $t1,  $t2, $t3
	add $t4,  $t0, $t1
	li $t5, 5
	sw $t5, 0($t4)
	li $t7, 1
	mul $t6,  $t7, $t3
	add $t8,  $t0, $t6
	sw $t3, 0($t8)
	li $s0, 2
	mul $t9,  $s0, $t3
	add $s1,  $t0, $t9
	sw $s0, 0($s1)
	li $s3, 3
	mul $s2,  $s3, $t3
	add $s4,  $t0, $s2
	sw $t7, 0($s4)
	mul $s5,  $t3, $t3
	add $s6,  $t0, $s5
	sw $s3, 0($s6)
	sw $t0, -28($fp)
	li $t0, 4
	addi $sp,  $sp, -4
	sw $t0, 0($sp)
	li $t1, 0
	addi $sp,  $sp, -4
	sw $t1, 0($sp)
	lw $t2, -28($fp)
	addi $sp,  $sp, -4
	sw $t2, 0($sp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal quicksort
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	move $t0, $v0
	lw $t1, -4($fp)
	li $t2, 0
	move $t1, $t2
	sw $t1, -4($fp)
label9:
	lw $t0, -4($fp)
	li $t1, 5
	sw $t0, -4($fp)
	bge $t0,  $t1, label8
	lw $t1, -4($fp)
	li $t2, 4
	mul $t0,  $t1, $t2
	lw $t4, -28($fp)
	add $t3,  $t4, $t0
	lw $t5, 0($t3)
	sw $t1, -4($fp)
	addi $sp,  $sp, -4
	sw $t3, -32($fp)
	sw $t4, -28($fp)
	addi $sp,  $sp, -4
	sw $t5, -36($fp)
	lw $t0, -36($fp)
	move $a0, $t0
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	lw $t1, -4($fp)
	li $t2, 1
	add $t1,  $t1, $t2
	sw $t1, -4($fp)
	j label9
label8:
	li $t0, 0
	move $v0, $t0
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
