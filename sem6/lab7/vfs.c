#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>

#define MYFS_MAGIC 0xBADCAFE
#define MAX_CACHE_SIZE 128
#define SLAB_NAME "myfs_slab"
#define OK 0

static struct kmem_cache *cache = NULL;
static void **cache_area = NULL;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogachenko Artem");

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};

static struct inode *myfs_make_inode(struct super_block *sb, int mode) {
    struct inode *ret = new_inode(sb);

    printk(KERN_DEBUG "+ MYFS: myfs_make_inode\n");


    if (ret) {
        inode_init_owner(&init_user_ns, ret, NULL, mode);
        printk(KERN_DEBUG "+ MYFS: inode_init_owner\n");

        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        // ret->i_no = 1;
    }
    return ret;
}

static void myfs_put_super(struct super_block *sb) {
    printk(KERN_DEBUG "+ MYFS: kill_sb\n");
}

static struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static int myfs_fill_sb(struct super_block *sb, void *data, int silent) {
    printk(KERN_DEBUG "+ MYFS: myfs_fill_sb\n");


    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC;
    sb->s_op = &myfs_super_ops;

    struct inode *root = myfs_make_inode(sb, S_IFDIR | 0755);

    if (!root) {
        printk(KERN_ERR "+ MYFS: inode allocation failed...\n");
        return -ENOMEM;
    }

    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;
    root->i_ino = 1;

    printk(KERN_DEBUG "+ MYFS: d_make_root\n");

    if (!(sb->s_root = d_make_root(root))) {
        printk(KERN_ERR "+ MYFS: root creation failed\n");
        iput(root);

        return -ENOMEM;
    }

    return OK;
}

static struct dentry *myfs_mount(struct file_system_type *type, int flags,
                                 char const *dev, void *data) {
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);
    printk(KERN_DEBUG "+ MYFS: myfs_mount\n");
    printk(KERN_DEBUG "+ MYFS: mount_nodev\n");


    if (IS_ERR(entry))
        printk(KERN_ERR "+ MYFS: mounting failed\n");
    else
        printk(KERN_DEBUG "+ MYFS: mounted!\n");

    return entry;
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "myfs",
    .mount = myfs_mount,
    .kill_sb = kill_anon_super,
    .fs_flags = FS_USERNS_MOUNT,
};

static int __init myfs_init(void) {
    int ret = register_filesystem(&myfs_type);
    printk(KERN_DEBUG "+ MYFS: register_filesystem\n");

    if (ret != 0) {
        printk(KERN_ERR "+ MYFS: Failed to register filesystem\n");
        return ret;
    }
    
    printk(KERN_DEBUG "+ MYFS: kmem_cache_create\n");
    
    if ((cache = kmem_cache_create(SLAB_NAME, sizeof(void *), 0, 0, NULL)) ==
        NULL) {
        printk(KERN_ERR "+ MYFS: Failed to create cache\n");

        return -ENOMEM;
    }

    printk(KERN_DEBUG "+ MYFS: kmem_cache_alloc\n");

    // if ((cache_area = kmem_cache_alloc(cache, 0)) == NULL) {
    //     printk(KERN_ERR "+ MYFS: Failed to allocate cache\n");
    //     kmem_cache_destroy(cache);
    //     return -ENOMEM;
    // }

    printk(KERN_DEBUG "+ MYFS: Module loaded\n");
    return OK;
}

static void __exit myfs_exit(void) {
    int ret = unregister_filesystem(&myfs_type);
    printk(KERN_DEBUG "+ MYFS: unregister_filesystem\n");

    if (ret != 0) {
        printk(KERN_ERR "+ MYFS: Can't unregister filesystem\n");
        return;
    }
    printk(KERN_DEBUG "+ MYFS: kmem_cache_free\n");

    // kmem_cache_free(cache_area, cache);

    printk(KERN_DEBUG "+ MYFS: kmem_cache_destroy\n");

    kmem_cache_destroy(cache);
    printk(KERN_DEBUG "+ MYFS: Module unloaded\n");
}

module_init(myfs_init);
module_exit(myfs_exit);

// touch file
// mkdir dir
// sudo mount -o loop -t myfs ./file ./dir
