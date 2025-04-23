import UIKit

class AppCell: UITableViewCell {
    static let identifier = "AppCell"
    
    // UI Elements
    private let appIconImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.contentMode = .scaleAspectFit
        imageView.layer.cornerRadius = 10
        imageView.clipsToBounds = true
        imageView.translatesAutoresizingMaskIntoConstraints = false
        return imageView
    }()
    
    private let appNameLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 16, weight: .medium)
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    private let bundleIdLabel: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 12)
        label.textColor = .gray
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    // MARK: - Initialization
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        setupUI()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupUI()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        // Add subviews
        contentView.addSubview(appIconImageView)
        contentView.addSubview(appNameLabel)
        contentView.addSubview(bundleIdLabel)
        
        // Set up constraints
        NSLayoutConstraint.activate([
            // App icon constraints
            appIconImageView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            appIconImageView.centerYAnchor.constraint(equalTo: contentView.centerYAnchor),
            appIconImageView.widthAnchor.constraint(equalToConstant: 40),
            appIconImageView.heightAnchor.constraint(equalToConstant: 40),
            
            // App name constraints
            appNameLabel.leadingAnchor.constraint(equalTo: appIconImageView.trailingAnchor, constant: 12),
            appNameLabel.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 12),
            appNameLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),
            
            // Bundle ID constraints
            bundleIdLabel.leadingAnchor.constraint(equalTo: appIconImageView.trailingAnchor, constant: 12),
            bundleIdLabel.topAnchor.constraint(equalTo: appNameLabel.bottomAnchor, constant: 4),
            bundleIdLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),
            bundleIdLabel.bottomAnchor.constraint(lessThanOrEqualTo: contentView.bottomAnchor, constant: -12)
        ])
        
        // Set up accessory type
        accessoryType = .disclosureIndicator
    }
    
    // MARK: - Configuration
    
    func configure(with app: App) {
        appNameLabel.text = app.name
        bundleIdLabel.text = app.bundleId
        
        if let icon = app.icon {
            appIconImageView.image = icon
        } else {
            // Set a placeholder icon
            appIconImageView.image = UIImage(systemName: "app.fill")
        }
    }
    
    override func prepareForReuse() {
        super.prepareForReuse()
        appIconImageView.image = nil
        appNameLabel.text = nil
        bundleIdLabel.text = nil
    }
}