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
main:
	addi $sp,  $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	addi $sp,  $sp, -4
	lw $t0, -4($fp)
	li $t1, 0
	move $t0, $t1
	addi $sp,  $sp, -4
	lw $t2, -8($fp)
	li $t3, 1
	move $t2, $t3
	addi $sp,  $sp, -4
	lw $t4, -12($fp)
	move $t4, $t1
	addi $sp,  $sp, -4
	sw $t0, -4($fp)
	sw $t2, -8($fp)
	sw $t4, -12($fp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	lw $t0, -16($fp)
	move $t0, $v0
	sw $t0, -16($fp)
label1:
	lw $t0, -12($fp)
	lw $t1, -16($fp)
	sw $t0, -12($fp)
	sw $t1, -16($fp)
	bge $t0,  $t1, label0
	addi $sp,  $sp, -4
	lw $t0, -20($fp)
	lw $t1, -4($fp)
	lw $t2, -8($fp)
	add $t0,  $t1, $t2
	sw $t0, -20($fp)
	sw $t1, -4($fp)
	sw $t2, -8($fp)
	lw $t0, -8($fp)
	move $a0, $t0
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	lw $t1, -4($fp)
	move $t1, $t0
	lw $t2, -20($fp)
	move $t0, $t2
	lw $t3, -12($fp)
	li $t4, 1
	add $t3,  $t3, $t4
	sw $t0, -8($fp)
	sw $t1, -4($fp)
	sw $t2, -20($fp)
	sw $t3, -12($fp)
	j label1
label0:
	li $t0, 0
	move $v0, $t0
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
