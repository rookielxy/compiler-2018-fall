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
fact:
	addi $sp,  $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	lw $t0, 8($fp)
	li $t1, 1
	sw $t0, 8($fp)
	bne $t0,  $t1, label1
	li $t0, 1
	move $v0, $t0
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
label1:
	lw $t1, 8($fp)
	li $t2, 1
	sub $t0,  $t1, $t2
	addi $sp,  $sp, -4
	sw $t0, -4($fp)
	sw $t1, 8($fp)
	lw $t0, -4($fp)
	addi $sp,  $sp, -4
	sw $t0, 0($sp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal fact
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	move $t0, $v0
	lw $t2, 8($fp)
	mul $t1,  $t2, $t0
	move $v0, $t1
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
	addi $sp,  $sp, -4
	sw $t1, -8($fp)
	sw $t2, 8($fp)
label0:
main:
	addi $sp,  $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp
	addi $sp,  $sp, -4
	addi $sp,  $sp, -4
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	lw $t0, -4($fp)
	move $t0, $v0
	li $t1, 1
	sw $t0, -4($fp)
	ble $t0,  $t1, label3
	lw $t0, -4($fp)
	addi $sp,  $sp, -4
	sw $t0, 0($sp)
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal fact
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	lw $t0, -8($fp)
	move $t0, $v0
	sw $t0, -8($fp)
	j label2
label3:
	lw $t0, -8($fp)
	li $t1, 1
	move $t0, $t1
	sw $t0, -8($fp)
label2:
	lw $t0, -8($fp)
	move $a0, $t0
	addi $sp,  $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp,  $sp, 4
	li $t1, 0
	move $v0, $t1
	move $sp, $fp
	lw $fp, 0($sp)
	addi $sp,  $sp, 4
	jr $ra
	sw $t0, -8($fp)
